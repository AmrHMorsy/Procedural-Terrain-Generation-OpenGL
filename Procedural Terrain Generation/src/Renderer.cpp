#include "Renderer.h"
#include "../external/include/stb_image_write.h"






Renderer::Renderer( GLFWwindow * window_): window(window_), avg_time(0.0f), counter(0), max_time(0.0f), min_time(1000.0f), key_pressed(false)
{
    camera = new Camera( window ) ;
    light = new Light(window) ;
    skybox = new Skybox( camera->get_view(), camera->get_projection(), camera->get_camera_position() ) ;
    shadow = new Shadow( window, light->get_projection(), light->get_view() ) ;
    terrain = new Terrain( window, camera->get_view(), camera->get_projection(), camera->get_camera_position(), light->get_view(), light->get_projection(), light->get_position(), light->get_color() ) ;
}

void Renderer::set()
{
    terrain->set( skybox ) ;
}

void Renderer::start_game_loop()
{
    update_view() ;
    while( !glfwWindowShouldClose(window) ){
        time = glfwGetTime() ;
        render_shadow() ;
        render_scene() ;
        glfwSwapBuffers(window) ;
        handle_events() ;
        time = glfwGetTime() - time ;
        compute_fps() ;
    }
}

void Renderer::render_shadow()
{
    // shadow->render() ;
    // terrain->render( shadow->get_shader_program() ) ;
    shadow->reset() ;
}

void Renderer::render_scene()
{
    skybox->activate() ;
    skybox->render() ;
    terrain->render( shadow ) ;
}

void Renderer::handle_events()
{
    glfwPollEvents() ;
    if( glfwGetKey(window, GLFW_KEY_UP) == GLFW_PRESS ){
        camera->look_around(false,false,true,false) ;
        update_view() ;
    }
    if( glfwGetKey(window, GLFW_KEY_DOWN) == GLFW_PRESS ){
        camera->look_around(false,false,false,true) ;
        update_view() ;
    }
    if( glfwGetKey(window, GLFW_KEY_LEFT) == GLFW_PRESS ){
        camera->look_around(true,false,false,false) ;
        update_view() ;
    }
    if( glfwGetKey(window, GLFW_KEY_RIGHT) == GLFW_PRESS ){
        camera->look_around(false,true,false,false) ;
        update_view() ;
    }
    if( glfwGetKey(window, GLFW_KEY_O) == GLFW_PRESS ){
        camera->move() ;
        update_view() ;
    }
}

void Renderer::compute_fps()
{
    avg_time += time ;
    if( time > max_time )
        max_time = time ;
    if( time < min_time )
        min_time = time ;
    counter++ ;
    if( counter == 10 ){
        avg_time /= 10.0f ;
        counter = 0 ;
        fps_max = static_cast<int>( std::round( 1.0f / min_time ) ) ;
        fps_avg = static_cast<int>( std::round( 1.0f / avg_time ) ) ;
        fps_min = static_cast<int>( std::round( 1.0f / max_time ) ) ;
        std::cout << "Min_FPS = \033[1;31m " << fps_min << "\033[0m     Avg_FPS = \033[1;31m " << fps_avg << "\033[0m     Max_FPS = \033[1;31m " << fps_max << "\033[0m" << '\r' << std::flush;
        avg_time = 0.0f ;
        max_time = 0.0f ; 
        min_time = 1000.0f ;
    }
}

void Renderer::update_view()
{
    terrain->update_view( camera->get_camera_position(), camera->get_view() ) ;
    skybox->update_view( camera->get_view(), camera->get_camera_position() ) ;
}

void Renderer::generate_error( std::string error_msg )
{
    std::cout << "\033[1;31m" << error_msg << "\033[0m" << std::endl;
    exit(1) ;
}

Renderer::~Renderer()
{
    delete terrain ;
    terrain = NULL ;
    delete shadow ;
    shadow = NULL ;
    delete light ;
    light = NULL ;
    delete camera ;
    camera = NULL ;
    delete skybox ;
    skybox = NULL ;
}
