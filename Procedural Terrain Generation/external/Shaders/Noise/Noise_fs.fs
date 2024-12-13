#version 330 core


in float height ;
in vec3 normal ;
in float ao ;

out vec4 fragment ;

void main()
{
    fragment = vec4( height, normal.x, normal.z, ao ) ;
}
