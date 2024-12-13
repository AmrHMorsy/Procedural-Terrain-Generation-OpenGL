#include "Skybox.h"
#define STB_IMAGE_IMPLEMENTATION
#include "../external/Include/stb_image.h"



Skybox::Skybox( glm::mat4 view, glm::mat4 projection, glm::vec3 camera ): resolution(512)
{
    skybox = new Shader("../external/Shaders/Skybox/Skybox_vs.vs", "../external/Shaders/Skybox/Skybox_fs.fs");
    equirect_to_cubemap = new Shader( "../external/Shaders/Skybox/EquirectToCubemap_vs.vs", "../external/Shaders/Skybox/EquirectToCubemap_fs.fs" ) ;
    irradiance = new Irradiance() ;
    equirectTocubemap_shader = equirect_to_cubemap->get_shader_program() ;
    skybox_shader = skybox->get_shader_program() ;
    bind_vertices() ;
    set_uniform_variables( view, projection, camera ) ;
    
    load_hdr_map() ;
}

void Skybox::load_hdr_map()
{
    int w, h, nc ;
    float * data = stbi_loadf("../assets/Sky/Sky.hdr", &w, &h, &nc, 0) ;
    if( !data )
        generate_error( "Loading HDR Environment Map Failed" ) ;
    glGenTextures(1,&hdr_texture) ;
    glBindTexture(GL_TEXTURE_2D, hdr_texture);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB16F, w, h, 0, GL_RGB, GL_FLOAT, data);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    stbi_image_free(data);
    
    create_framebuffers() ;
}

void Skybox::create_framebuffers()
{
    glGenFramebuffers(1, &FBO);
    glGenRenderbuffers(1, &RBO);
    glBindFramebuffer(GL_FRAMEBUFFER, FBO);
    glBindRenderbuffer(GL_RENDERBUFFER, RBO);
    glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH_COMPONENT24, resolution, resolution );
    glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_RENDERBUFFER, RBO);
    
    setup_cubemap() ;
}

void Skybox::setup_cubemap()
{
    glGenTextures(1, &cubemap) ;
    glBindTexture(GL_TEXTURE_CUBE_MAP,cubemap) ;
    for( unsigned int i = 0 ; i < 6 ; i++ )
        glTexImage2D( GL_TEXTURE_CUBE_MAP_POSITIVE_X+i, 0, GL_RGB16F, resolution, resolution, 0, GL_RGB, GL_FLOAT, nullptr ) ;
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);

    convert_hdrmap_to_cubemap() ;
}

void Skybox::convert_hdrmap_to_cubemap()
{
    glm::mat4 equirectTocubemap_proj = glm::perspective(glm::radians(90.0f), 1.0f, 0.1f, 10.0f);
    glm::mat4 equirectTocubemap_views[6] = {
        glm::lookAt(glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(1.0f, 0.0f,  0.0f), glm::vec3(0.0f, -1.0f,  0.0f)),
        glm::lookAt(glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(-1.0f, 0.0f,  0.0f), glm::vec3(0.0f, -1.0f,  0.0f)),
        glm::lookAt(glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(0.0f, 1.0f,  0.0f), glm::vec3(0.0f,  0.0f,  1.0f)),
        glm::lookAt(glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(0.0f,-1.0f, 0.0f), glm::vec3(0.0f,  0.0f, -1.0f)),
        glm::lookAt(glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(0.0f, 0.0f, 1.0f), glm::vec3(0.0f, -1.0f,  0.0f)),
        glm::lookAt(glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(0.0f, 0.0f,-1.0f), glm::vec3(0.0f, -1.0f,  0.0f))};
    glUseProgram(equirectTocubemap_shader) ;
    glUniform1i(glGetUniformLocation(equirectTocubemap_shader, "skybox"), 0) ;
    glUniformMatrix4fv(glGetUniformLocation(equirectTocubemap_shader, "projection"), 1, GL_FALSE, &equirectTocubemap_proj[0][0]) ;
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, hdr_texture);
    glViewport(0, 0, resolution, resolution) ;
    glBindFramebuffer( GL_FRAMEBUFFER, FBO ) ;
    for ( unsigned int i = 0; i < 6; i++ ){
        glUniformMatrix4fv(glGetUniformLocation(equirectTocubemap_shader, "view"), 1, GL_FALSE, &equirectTocubemap_views[i][0][0]) ;
        glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, cubemap, 0);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
        render() ;
    }
    glBindFramebuffer(GL_FRAMEBUFFER, 0);
    irradiance->set(vao, cubemap, FBO, RBO ) ;
    irradiance->create_irradiance_map() ;
}

void Skybox::render()
{
    glDepthFunc(GL_LEQUAL) ;
    glBindVertexArray(vao) ;
    activate_texture() ;
    glDrawArrays(GL_TRIANGLES, 0, 36);
    glBindVertexArray(0) ;
    glDepthFunc(GL_LESS);
}

void Skybox::bind_vertices()
{
    float skybox_vertices[108] = { -1.0f,  1.0f, -1.0f, -1.0f, -1.0f, -1.0f, 1.0f, -1.0f, -1.0f, 1.0f, -1.0f, -1.0f, 1.0f,  1.0f, -1.0f, -1.0f,  1.0f, -1.0f, -1.0f, -1.0f,  1.0f, -1.0f, -1.0f, -1.0f, -1.0f,  1.0f, -1.0f, -1.0f,  1.0f, -1.0f, -1.0f,  1.0f,  1.0f, -1.0f, -1.0f,  1.0f, 1.0f, -1.0f, -1.0f, 1.0f, -1.0f,  1.0f, 1.0f,  1.0f,  1.0f, 1.0f,  1.0f,  1.0f, 1.0f,  1.0f, -1.0f, 1.0f, -1.0f, -1.0f, -1.0f, -1.0f,  1.0f, -1.0f,  1.0f,  1.0f, 1.0f,  1.0f,  1.0f, 1.0f,  1.0f,  1.0f, 1.0f, -1.0f,  1.0f, -1.0f, -1.0f,  1.0f, -1.0f,  1.0f, -1.0f, 1.0f,  1.0f, -1.0f, 1.0f,  1.0f,  1.0f, 1.0f,  1.0f,  1.0f, -1.0f,  1.0f,  1.0f, -1.0f,  1.0f, -1.0f, -1.0f, -1.0f, -1.0f, -1.0f, -1.0f,  1.0f, 1.0f, -1.0f, -1.0f, 1.0f, -1.0f, -1.0f, -1.0f, -1.0f,  1.0f, 1.0f, -1.0f,  1.0f };
    glUseProgram(skybox_shader) ;
    glGenVertexArrays(1, &vao) ;
    glGenBuffers(1, &vbo) ;
    glBindVertexArray(vao) ;
    glBindBuffer(GL_ARRAY_BUFFER, vbo) ;
    glBufferData(GL_ARRAY_BUFFER, 108*sizeof(float), skybox_vertices, GL_STATIC_DRAW) ;
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, (3*sizeof(float)), (void*)0) ;
    glEnableVertexAttribArray(0) ;
}

void Skybox::set_uniform_variables( glm::mat4 view, glm::mat4 projection, glm::vec3 camera )
{
    glUseProgram(skybox_shader) ;
    view = glm::mat4(glm::mat3(view)) ;
    glUniformMatrix4fv(glGetUniformLocation(skybox_shader, "view"), 1, GL_FALSE, &view[0][0]) ;
    glUniform1i(glGetUniformLocation(skybox_shader, "skybox"), 0) ;
    glUniformMatrix4fv(glGetUniformLocation(skybox_shader, "projection"), 1, GL_FALSE, &projection[0][0]) ;
    glUniform3fv(glGetUniformLocation(skybox_shader, "camera"), 1, &camera[0]) ;
}

void Skybox::update_view( glm::mat4 view, glm::vec3 camera )
{
    glUseProgram(skybox_shader) ;
    view = glm::rotate( view, glm::radians(180.0f), glm::vec3(1.0f, 0.0f, 0.0f) ) ;
    view = glm::mat4(glm::mat3(view)) ;
    glUniformMatrix4fv(glGetUniformLocation(skybox_shader, "view"), 1, GL_FALSE, &view[0][0]) ;
    glUniform3fv(glGetUniformLocation(skybox_shader, "camera"), 1, &camera[0]) ;
    glm::vec3 l( camera.x, 100.0f , camera.z ) ;
    glUniform3fv( glGetUniformLocation(skybox_shader, "light_position" ), 1, &l[0] ) ;
}

void Skybox::activate()
{
    glUseProgram( skybox_shader ) ;
}

void Skybox::activate_texture()
{
    glActiveTexture( GL_TEXTURE0 ) ;
    glBindTexture( GL_TEXTURE_CUBE_MAP, cubemap ) ;
}

void Skybox::activate_irradiance()
{
    irradiance->activate_irradiance() ;
}

void Skybox::generate_error( std::string msg )
{
    std::cout << msg << std::endl ;
    exit(1) ;
}

Skybox::~Skybox()
{
    delete skybox ;
    skybox = NULL ;
    delete equirect_to_cubemap ;
    skybox = NULL ;
    delete irradiance ;
    irradiance = NULL ;
    glDeleteVertexArrays(1, &vao);
    glDeleteBuffers(1, &vbo);
    glDeleteBuffers(1, &cubemap);
    glDeleteBuffers(1, &hdr_texture);
}
