#include "Noise.h"



Noise::Noise( GLFWwindow * w, std::vector<float> vertices, std::vector<unsigned int> indices, unsigned int n, glm::mat4 * models, glm::vec3 max, glm::vec3 min): window(w), num_patches(n), resolution(8192)
{
    load_shaders() ;
    generate_buffers() ;
    bind_vertices( vertices ) ;
    bind_models( models ) ;
    bind_indices( indices ) ;
    set_uniform_variables( max, min ) ;
    create_height_map() ;
    create_framebuffer() ;
    bake_noise() ;
    compute_num_mip_levels() ;
    compute_data() ;
}

void Noise::load_shaders()
{
    shader = new Shader( "../external/shaders/Noise/Noise_vs.vs", "../external/shaders/Noise/Noise_fs.fs", "../external/shaders/Noise/Noise_tcs.tcs", "../external/shaders/Noise/Noise_tes.tes" ) ;
    shader_program = shader->get_shader_program() ;
    delete shader ;
    shader = NULL ;
}

void Noise::generate_buffers()
{
    glPatchParameteri( GL_PATCH_VERTICES, 3 ) ;
    glGenVertexArrays( 1, &vao ) ;
    glGenBuffers( 1, &vbo ) ;
    glGenBuffers( 1, &ebo ) ;
    glGenBuffers( 1, &model_vbo ) ;
}

void Noise::bind_vertices( std::vector<float> vertices )
{
    glBindVertexArray( vao ) ;
    glBindBuffer( GL_ARRAY_BUFFER, vbo ) ;
    glBufferData(GL_ARRAY_BUFFER, vertices.size()*sizeof(float), vertices.data(), GL_STATIC_DRAW);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, (3*sizeof(float)), (void*)0) ;
    glEnableVertexAttribArray(0) ;
}

void Noise::bind_models( glm::mat4 * models )
{
    glBindVertexArray(vao) ;
    glBindBuffer(GL_ARRAY_BUFFER, model_vbo);
    glBufferData(GL_ARRAY_BUFFER, num_patches * sizeof(glm::mat4), &models[0], GL_STATIC_DRAW) ;
    for (int i = 0; i < 4; i++){
        glVertexAttribPointer(1+i, 4, GL_FLOAT, GL_FALSE, sizeof(glm::mat4), (void*)(sizeof(glm::vec4) * i)) ;
        glEnableVertexAttribArray(1+i) ;
        glVertexAttribDivisor(1+i, 1) ;
    }
    glBindVertexArray(0) ;
}

void Noise::bind_indices( std::vector<unsigned int> indices )
{
    glBindVertexArray(vao) ;
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ebo) ;
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, indices.size()*sizeof(int), indices.data(), GL_STATIC_DRAW) ;
    glBindVertexArray(0) ;
    num_indices = indices.size() ;
}

void Noise::set_uniform_variables( glm::vec3 max_position, glm::vec3 min_position )
{
    glUseProgram( shader_program ) ;
    glUniform3fv( glGetUniformLocation(shader_program, "max_position"), 1, &max_position[0] ) ;
    glUniform3fv( glGetUniformLocation(shader_program, "min_position"), 1, &min_position[0] ) ;
}

void Noise::create_height_map()
{
    glGenTextures( 1, &height_map ) ;
    glBindTexture( GL_TEXTURE_2D, height_map ) ;
    glTexImage2D( GL_TEXTURE_2D, 0, GL_RGBA32F, resolution, resolution, 0, GL_RGBA, GL_FLOAT, NULL ) ;
    glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR ) ;
    glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR ) ;
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_MIRRORED_REPEAT) ;
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_MIRRORED_REPEAT) ;
    glGenerateMipmap( GL_TEXTURE_2D ) ;
    glBindTexture( GL_TEXTURE_2D, 0 ) ;
}

void Noise::create_framebuffer()
{
    glGenFramebuffers( 1, &fbo ) ;
    glBindFramebuffer( GL_FRAMEBUFFER, fbo ) ;
    glFramebufferTexture2D( GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, height_map, 0 ) ;
    GLenum status = glCheckFramebufferStatus( GL_FRAMEBUFFER ) ;
    if( status != GL_FRAMEBUFFER_COMPLETE ){
        std::cerr << "Framebuffer error: " << status << std::endl ;
        generate_error( "Framebuffer is not complete" ) ;
    }
}

void Noise::bake_noise()
{
    glUseProgram( shader_program ) ;
    glBindFramebuffer( GL_FRAMEBUFFER, fbo ) ;
    glViewport( 0, 0, resolution, resolution ) ;
    glDisable(GL_DEPTH_TEST);

    glClearColor( 0.0f, 0.0f, 0.0f, 1.0f ) ;
    glClear( GL_COLOR_BUFFER_BIT ) ;
    
    glBindVertexArray( vao ) ;
    glDrawElementsInstanced( GL_PATCHES, num_indices, GL_UNSIGNED_INT, 0, num_patches ) ;
    glBindVertexArray(0) ;
    
    glBindFramebuffer(GL_FRAMEBUFFER, 0) ;
    glClear(GL_COLOR_BUFFER_BIT) ;
    glClearColor(0.0f, 0.0f, 0.0f,1.0f) ;
    int width, height ;
    glfwGetFramebufferSize(window, &width, &height) ;
    glViewport(0, 0, width, height) ;
}

void Noise::activate_textures()
{
    glActiveTexture( GL_TEXTURE0 ) ;
    glBindTexture( GL_TEXTURE_2D, height_map ) ;
}

void Noise::compute_num_mip_levels()
{
    num_mip_levels = floor( log2( resolution ) ) + 1.0f ;
}

void Noise::compute_data()
{
    glBindTexture(GL_TEXTURE_2D, height_map) ;
    std::vector<float> height_data(resolution * resolution * 4) ;
    glGetTexImage(GL_TEXTURE_2D, 0, GL_RGBA, GL_FLOAT, height_data.data());
    glBindTexture(GL_TEXTURE_2D, 0);
    min_second_derivative = std::numeric_limits<float>::max() ;
    max_second_derivative = std::numeric_limits<float>::min() ;
    min_first_derivative = std::numeric_limits<float>::max() ;
    max_first_derivative = std::numeric_limits<float>::min() ;
    
    for( int i = 0 ; i < height_data.size() ; i += 4 ){
        float second_derivative = height_data[i+3] ;
        float first_derivative = sqrt((height_data[i+1]*height_data[i+1]) + (height_data[i+2]*height_data[i+2])) ;
        if( second_derivative > max_second_derivative )
            max_second_derivative = second_derivative ;
        if( second_derivative < min_second_derivative )
            min_second_derivative = second_derivative ;
        if( first_derivative > max_first_derivative )
            max_first_derivative = first_derivative ;
        if( first_derivative < min_first_derivative )
            min_first_derivative = first_derivative ;
    }
}

float Noise::get_num_mip_levels()
{
    return num_mip_levels ;
}

float Noise::get_min_second_derivative()
{
    return min_second_derivative ;
}

float Noise::get_max_second_derivative()
{
    return max_second_derivative ;
}

float Noise::get_min_first_derivative()
{
    return min_first_derivative ;
}

float Noise::get_max_first_derivative()
{
    return max_first_derivative ;
}

void Noise::generate_error( std::string msg )
{
    std::cout << "\n\033[1;31m" << msg << "\033[0m\n" << std::endl;

    exit(1) ;
}

Noise::~Noise()
{
    delete shader ;
    shader = NULL ;
    glDeleteVertexArrays( 1, &vao ) ;
    glDeleteBuffers( 1, &vbo ) ;
    glDeleteBuffers( 1, &ebo ) ;
}
