#version 330 core

layout( location = 0 ) in vec3 vertex ;
layout( location = 1 ) in mat4 model ;

out vec3 world_position ;

void main()
{
    world_position = ( model * vec4( vertex, 1.0 ) ).xyz ;
}
