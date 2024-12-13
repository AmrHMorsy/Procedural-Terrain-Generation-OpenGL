#version 330 core

layout(location = 0) in vec3 vertex ;
layout(location = 1) in mat4 model ;

out float visible ;
out vec3 world_position ;

uniform mat4 view ;
uniform mat4 projection ;

float compute_visibility( vec3 position )
{
    vec4 clip_space_pos = projection * view * vec4(position, 1.0);
    vec3 ndc = clip_space_pos.xyz / clip_space_pos.w ;

    if( ndc.x < -1.0 || ndc.x > 1.0 || ndc.y < -3.0 || ndc.y > 1.0 || ndc.z < -1.0 || ndc.z > 1.0)
        return 0.0f ;
    
    return 1.0f ;
}

void main()
{
    world_position = ( model * vec4( vertex, 1.0 ) ).xyz ;
    visible = compute_visibility( world_position ) ;
}
