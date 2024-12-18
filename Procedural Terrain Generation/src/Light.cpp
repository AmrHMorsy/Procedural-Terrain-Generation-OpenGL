#include "Light.h"





Light::Light( GLFWwindow * window )
{
    fov = 80.0f ;
    light_near_plane = -100.0f ;
    light_far_plane = 100.0f ;
    light_up = glm::vec3(0.0f, 0.0f, -1.0f) ;
    light_direction = glm::vec3(0.0f,-1.0f,0.0f) ;
    light_position = glm::vec3( 0.0f, 100.0f, 0.0f ) ;
    light_color = glm::vec3( 20.0f, 20.0f, 20.0f ) ;
    glfwGetFramebufferSize(window, &width, &height);
    set_view() ;
    set_projection() ;
}

void Light::set_projection()
{
//    projection = glm::ortho(-8.0f, 8.0f, -8.0f, 8.0f, 1.0f, 100.0f) ;
    projection = glm::perspective(glm::radians(fov), (float)width /(float)height, 1.0f, 1000.0f);
}

void Light::set_view()
{
    view = glm::lookAt(light_position, light_position+light_direction, light_up) ;
}

void Light::move( glm::vec3 direction )
{
    light_position = light_position + direction ;
    set_view() ;
    set_projection() ;
}

glm::mat4 Light::get_projection()
{
    return projection ;
}

glm::mat4 Light::get_view()
{
    return view ;
}

glm::vec3 Light::get_position()
{
    return light_position ;
}

glm::vec3 Light::get_color()
{
    return light_color ;
}

Light::~Light(){}
