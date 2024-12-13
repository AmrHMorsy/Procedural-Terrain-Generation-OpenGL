#version 330 core


in vec3 world_position ;
in vec4 fragment_position_light_space ;
in float height ;
in vec3 normal ;
in float ao ;
in vec3 albedo ;
in float roughness ;
in float fog_density ;
in float g ;
in float scattering_coefficient ;
in vec3 fog_color ;

out vec4 fragment ;

uniform vec3 camera ;
uniform vec3 light_color ;
uniform vec3 light_position ;

uniform samplerCube irradiance_map ;
uniform sampler2D shadow_map ;
uniform samplerCube prefilter_map;
uniform sampler2D brdf_integration_map ;

const float PI = 3.14159265359 ;

vec3 gamma_correct( vec3 color )
{
    color = pow(color, vec3(1.0/2.2)) ;
    
    return color ;
}

vec3 HDR_tonemapping( vec3 color )
{
    color = color / (color + vec3(1.0));
    
    return color ;
}

float geometry_schlick( float dot_product, float roughness )
{
    float r = (roughness + 1.0) ;
    float k_direct = (r*r) / 8.0 ;
    float numerator = dot_product ;
    float denominator = dot_product * (1.0-k_direct) + k_direct  ;
    
    return numerator / denominator ;
}

float geometry_smith( vec3 normal, vec3 view, vec3 light, float roughness )
{
    float normal_dot_view = max(dot(normal, view), 0.0);
    float normal_dot_light = max(dot(normal, light), 0.0);

    return geometry_schlick(normal_dot_light,roughness) * geometry_schlick(normal_dot_view,roughness) ;
}

float reitz_ndf( vec3 normal, vec3 halfway, float roughness )
{
    float r = roughness * roughness ;
    float a = r * r ;
    float normal_dot_halfway = max( dot(normal,halfway), 0.0 ) ;
    float normal_dot_halfway_sq = normal_dot_halfway * normal_dot_halfway ;
    float numerator = a ;
    float denominator = (normal_dot_halfway_sq * (a - 1.0) + 1.0) ;
    denominator = PI * denominator * denominator ;
    
    return numerator / denominator ;
}

vec3 frensel_schlick( float cos_theta )
{
    vec3 base_reflectivity = vec3(0.02) ;
    vec3 ratio = base_reflectivity + (1.0 - base_reflectivity) * pow(clamp(1.0 - cos_theta, 0.0, 1.0), 5.0);
    
    return ratio ;
}

vec3 frensel_schlick_roughness(float cos_theta, float roughness )
{
    vec3 base_reflectivity = vec3(0.02) ;
    return base_reflectivity + (max(vec3(1.0 - roughness), base_reflectivity) - base_reflectivity) * pow(clamp(1.0 - cos_theta, 0.0, 1.0), 5.0);
}

vec3 cook_torrance_brdf( vec3 normal, vec3 halfway, vec3 view, vec3 light, vec3 albedo, float roughness )
{
    vec3 lambertian_diffuse = albedo / PI ;
    vec3 frensel = frensel_schlick( max(dot(halfway,view),0.0) ) ;
    vec3 ks = frensel ;
    vec3 kd = vec3(1.0) - ks ;
    float ndf = reitz_ndf(normal, halfway, roughness) ;
    float geometry = geometry_smith(normal, view, light, roughness) ;
    float normal_dot_view = max( dot(normal, view), 0.0 );
    float normal_dot_light = max( dot(normal, light), 0.0 );
    vec3 specular = (ndf*geometry*frensel) / ((4.0*normal_dot_view*normal_dot_light)+0.0001 ) ;
    vec3 brdf = ( kd * lambertian_diffuse ) + ( ks * specular ) ;
    
    return brdf ;
}

vec3 calculate_radiance( vec3 normal, vec3 view, vec3 albedo, float roughness )
{
    vec3 out_radiance = vec3(0.0) ;
    vec3 light = normalize( light_position - world_position ) ;
    vec3 halfway = normalize( view + light );
    float dist = length( light_position - world_position );
    float attenuation = 1000.0f / (dist * dist);
    vec3 in_radiance = light_color * attenuation;
    float cos_theta = max( dot(normal, light), 0.0 ) ;
    vec3 brdf = cook_torrance_brdf( normal, halfway, view, light, albedo, roughness ) ;
    out_radiance +=  brdf * in_radiance * cos_theta ;
    
    return out_radiance ;
}

vec3 calculate_ambient( vec3 normal, vec3 view, vec3 albedo, float ao, float roughness )
{
    vec3 kS = frensel_schlick_roughness( max(dot(normal, view), 0.0), roughness );
    vec3 kD = 1.0 - kS ;
    vec3 irradiance = texture(irradiance_map, normal).rgb ;
    vec3 diffuse = irradiance * albedo ;
    vec3 R = reflect(-view, normal);
    const float MAX_REFLECTION_LOD = 4.0;
    vec3 prefilteredColor = textureLod(prefilter_map, R,  roughness * MAX_REFLECTION_LOD).rgb;
    vec2 brdf = texture(brdf_integration_map, vec2(max(dot(normal, view), 0.0), roughness)).rg;
    vec3 specular = prefilteredColor * (kS * brdf.x + brdf.y) ;
    vec3 ambient = (kD * diffuse+specular) * ao ; 
    
    return ambient ;
}
  
float compute_shadow()
{
    vec3 normalized_device_coordinates = fragment_position_light_space.xyz / fragment_position_light_space.w ;
    normalized_device_coordinates = normalized_device_coordinates * 0.5 + 0.5 ;
    float closest_depth = texture(shadow_map, normalized_device_coordinates.xy).r ;
    float current_depth = normalized_device_coordinates.z ;
    float bias = 0.00 ;
    
    return ((current_depth - bias) < closest_depth) ? 1.0 : 0.0 ;
}

float Henyey_Greenstein_phase(float cos_theta, float g)
{
    float g2 = g * g;
    
    return (1.0 - g2) / pow(1.0 + g2 - 2.0 * g * cos_theta, 1.5) / (4.0 * PI) ;
}

float compute_fog()
{
    int num_steps = 1 ;
    float total_scattering = 0.0f ;
    float total_transmittance = 1.0 ;
    vec3 position = camera ;
    vec3 ray_direction = normalize(world_position - camera) ;
    float step_size = length(world_position - camera) / num_steps ;

    for( int i = 0; i < num_steps; i++ ){
        position += ray_direction * step_size ;
        float segment_transmittance = exp( -fog_density * step_size ) ;
        vec3 light_direction = normalize( light_position - position ) ;
        vec3 scatter_direction = normalize(camera - position) ;
        float cos_theta = dot( normalize(light_direction), scatter_direction ) ;
        float phase_contribution = Henyey_Greenstein_phase(cos_theta, g) * scattering_coefficient;
        total_scattering += total_transmittance * phase_contribution * step_size ;
        total_transmittance *= segment_transmittance ;
    }

    return clamp( total_scattering, 0.0f, 1.0f) ;
}

vec4 pbr()
{
    vec3 color ;
    float distance_to_camera = length( camera - world_position ) ;
    if( distance_to_camera < 50.0f ){
        vec3 view = normalize( camera - world_position ) ;
        vec3 radiance = calculate_radiance( normal, view, albedo, roughness ) ;
        vec3 ambient = calculate_ambient( normal, view, albedo, ao, roughness ) ;
        color = radiance + ambient ;
        color = HDR_tonemapping( color ) ;
        color = gamma_correct( color ) ;
    }
    else
        color = albedo ;
    
    return vec4( color, 1.0f ) ;
}

void main()
{
    fragment = pbr() ;
    // fragment = mix( pbr(), vec4(fog_color,1.0f), compute_fog() ) ;
}
