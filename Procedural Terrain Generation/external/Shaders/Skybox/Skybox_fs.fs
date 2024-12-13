#version 330 core

out vec4 FragColor ;

in vec3 TexCoords ;
in vec3 world_position ;

uniform mat4 view ;
uniform vec3 camera ;
uniform samplerCube skybox ;
uniform vec3 light_position ;

const float PI = 3.14159265359 ;

float Henyey_Greenstein_phase( float cos_theta, float g )
{
    float g2 = g * g;
    return (1.0 - g2) / pow(1.0 + g2 - 2.0 * g * cos_theta, 1.5) / (4.0 * PI) ;
}

float compute_fog()
{
    float total_scattering = 0.0f ;
    int num_steps = 1 ;
    float density = 10.0f ;
    vec3 position = camera ;
    vec3 ray_direction = normalize( world_position - camera ) ;
//    float step_size = length(world_position - camera) / num_steps ;
    float max_distance = 10.0f ;
    float step_size = min(length(world_position - camera), max_distance) / num_steps;
    float total_transmittance = 1.0 ;
    float g = 0.5f ;
    float scattering_coefficient = 5.0f ;

    for (int i = 0; i < num_steps; i++) {
        position += ray_direction * step_size ;
        float segment_transmittance = exp(-density * step_size) ;
        vec3 light_direction = normalize( light_position - position ) ;
        vec3 scatter_direction = normalize( camera - position ) ;
        float cos_theta = dot(normalize(light_direction), scatter_direction) ;
        float phase_contribution = Henyey_Greenstein_phase(cos_theta, g) * scattering_coefficient;
        total_scattering += total_transmittance * phase_contribution * step_size ;
        total_transmittance *= segment_transmittance ;
    }
    return clamp( total_scattering, 0.0f, 1.0f ) ;
}

void main()
{
    vec4 color = texture(skybox, TexCoords) ;
    color.rgb = color.rgb * pow(2.0, -2.0f);
    color.rgb = color.rgb / (color.rgb + vec3(1.0));
    color.rgb = pow(color.rgb, vec3(1.0/2.2));
    color.rgb = color.rgb * 1.5f ;

    // float fog = compute_fog() ;
    // fog *= exp( (world_position.y) ) ;
    
    FragColor = color ;
    // FragColor = mix( color, vec4(vec3(0.5),1.0f), clamp(fog, 0.0f, 1.0f) ) ;
}
