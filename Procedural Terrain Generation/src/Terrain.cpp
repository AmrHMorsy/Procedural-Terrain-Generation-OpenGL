#include "Terrain.h"





Terrain::Terrain( GLFWwindow * w, glm::mat4 v, glm::mat4 proj, glm::vec3 c, glm::mat4 lightview, glm::mat4 lightproj, glm::vec3 lightpos, glm::vec3 lightcol ): view(v), projection(proj), camera(c), light_view(lightview), light_projection(lightproj), light_position(lightpos), light_color(lightcol), num_patches(10000), vertices({-1, 0, 1, 1, 0, 1, 1, 0, -1, -1, 0, -1}), indices({0, 1, 2, 0, 2, 3}), row(std::sqrt(num_patches)-1), index(-1), current_i(std::sqrt(num_patches)/2), index2(std::sqrt(num_patches)), row2(0), col(std::sqrt(num_patches)-1), cindex(-1), current_j(std::sqrt(num_patches)/2), cindex2(std::sqrt(num_patches)), col2(0), max_x(-std::numeric_limits<double>::infinity()), max_z(-std::numeric_limits<double>::infinity()), min_x(std::numeric_limits<double>::infinity()), min_z(std::numeric_limits<double>::infinity()), camera_grid_row(current_i), camera_grid_col(current_j), s( std::sqrt(num_patches) )
{
    load_shaders() ;
    generate_buffers() ;
    set_models() ;
    bind_vertices( vertices ) ;
    bind_models() ;
    bind_indices( indices ) ;
    noise = new Noise( w, vertices, indices, num_patches, models, max_position, min_position ) ;
    max_lod = noise->get_num_mip_levels() ;
    min_second_derivative = noise->get_min_second_derivative() ;
    max_second_derivative = noise->get_max_second_derivative() ;
    min_first_derivative = noise->get_min_first_derivative() ;
    max_first_derivative = noise->get_max_first_derivative() ;
    color = new Color( w, vertices, indices, num_patches, models, max_position, min_position ) ;
    set_uniform_variables() ;
}

void Terrain::set( Skybox * skybox )
{
    this->skybox = skybox ;
}

void Terrain::load_shaders()
{
    shader = new Shader( "../external/shaders/Terrain/Terrain_vs.vs", "../external/shaders/Terrain/Terrain_fs.fs", "../external/shaders/Terrain/Terrain_tcs.tcs", "../external/shaders/Terrain/Terrain_tes.tes" ) ;
    shader_program = shader->get_shader_program() ;
}

void Terrain::generate_buffers()
{
    glPatchParameteri( GL_PATCH_VERTICES, 3 ) ;
    glGenVertexArrays( 1, &vao ) ;
    glGenBuffers( 1, &vbo ) ;
    glGenBuffers( 1, &ebo ) ;
    glGenBuffers( 1, &model_vbo ) ;
}

void Terrain::bind_vertices( std::vector<float> vertices )
{
    glBindVertexArray( vao ) ;
    glBindBuffer( GL_ARRAY_BUFFER, vbo ) ;
    glBufferData(GL_ARRAY_BUFFER, vertices.size()*sizeof(float), vertices.data(), GL_STATIC_DRAW);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, (3*sizeof(float)), (void*)0) ;
    glEnableVertexAttribArray(0) ;
}

void Terrain::bind_models()
{
    glBindVertexArray( vao ) ;
    glBindBuffer( GL_ARRAY_BUFFER, model_vbo ) ;
    glBufferData( GL_ARRAY_BUFFER, num_patches * sizeof(glm::mat4), &models[0], GL_STATIC_DRAW );
    for( int i = 0; i < 4 ; i++ ){
        glVertexAttribPointer( 1+i, 4, GL_FLOAT, GL_FALSE, sizeof(glm::mat4), (void*)(sizeof(glm::vec4) * i) ) ;
        glEnableVertexAttribArray( 1+i ) ;
        glVertexAttribDivisor( 1+i, 1 ) ;
    }
    glBindVertexArray(0) ;
}

void Terrain::bind_indices( std::vector<unsigned int> indices )
{
    glBindVertexArray( vao ) ;
    glBindBuffer( GL_ELEMENT_ARRAY_BUFFER, ebo ) ;
    glBufferData( GL_ELEMENT_ARRAY_BUFFER, indices.size()*sizeof(int), indices.data(), GL_STATIC_DRAW ) ;
    glBindVertexArray(0) ;
    num_indices = indices.size() ;
}

void Terrain::set_uniform_variables()
{
    glUseProgram( shader_program ) ;
    glUniform1i( glGetUniformLocation(shader_program, "height_map" ), 0 ) ;
    glUniform1i( glGetUniformLocation(shader_program, "color_map" ), 1 ) ;
    glUniform1i( glGetUniformLocation(shader_program, "irradiance_map" ), 7 ) ;
    glUniform1i( glGetUniformLocation(shader_program, "prefilter_map" ), 8 ) ;
    glUniform1i( glGetUniformLocation(shader_program, "brdf_integration_map" ), 9 ) ;
    glUniform1i( glGetUniformLocation(shader_program, "shadow_map"), 10 ) ;
    glUniform1f( glGetUniformLocation(shader_program, "max_lod"), max_lod ) ;
    glUniformMatrix4fv( glGetUniformLocation(shader_program, "projection"), 1, GL_FALSE, &projection[0][0] ) ;
    glUniform3fv( glGetUniformLocation(shader_program, "light_color" ), 1, &light_color[0] ) ;
    glUniform3fv( glGetUniformLocation(shader_program, "light_position"), 1, &light_position[0] );
    glUniformMatrix4fv( glGetUniformLocation(shader_program, "light_view"), 1, GL_FALSE, &light_view[0][0] ) ;
    glUniformMatrix4fv( glGetUniformLocation(shader_program, "light_projection"), 1, GL_FALSE, &light_projection[0][0] ) ;
    glUniform3fv( glGetUniformLocation(shader_program, "camera"), 1, &camera[0] ) ;
    glUniform3fv( glGetUniformLocation(shader_program, "max_position"), 1, &max_position[0] ) ;
    glUniform3fv( glGetUniformLocation(shader_program, "min_position"), 1, &min_position[0] ) ;
    glUniform1f( glGetUniformLocation(shader_program, "max_second_derivative"), max_second_derivative ) ;
    glUniform1f( glGetUniformLocation(shader_program, "min_second_derivative"), min_second_derivative ) ;
    glUniform1f( glGetUniformLocation(shader_program, "max_first_derivative"), max_first_derivative ) ;
    glUniform1f( glGetUniformLocation(shader_program, "min_first_derivative"), min_first_derivative ) ;
}

void Terrain::set_models()
{
    float x, z ;
    int counter = 0 ;
    // int s = std::sqrt(num_patches) ;
    models = new glm::mat4[ num_patches ] ;
    for( int i = 0; i < s ; i++ ) {
        z = (i - s / 2) * 2.0f ;
        if( z > max_z )
            max_z = z ;
        if( z < min_z )
            min_z = z ;
        for( int j = 0; j < s ; j++ ) {
            x = (j - s / 2) * 2.0f ;
            if( x > max_x )
                max_x = x ;
            if( x < min_x )
                min_x = x ;
            models[ counter ] = glm::translate( glm::mat4( 1.0 ), glm::vec3( x, 0.0f, z ) ) ;
            counter++ ;
        }
    }
    max_position = glm::vec3( 1.0f, 0.0f, 1.0f ) + glm::vec3( max_x, 0.0f, max_z ) ;
    min_position = glm::vec3( -1.0f, 0.0f, -1.0f ) + glm::vec3( min_x, 0.0f, min_z ) ;
}

void Terrain::render( Shadow * shadow )
{
    glUseProgram( shader_program ) ;
    expand_terrain() ;
    activate_textures() ;
    shadow->activate_texture() ;
    glBindVertexArray( vao ) ;
    glDrawElementsInstanced( GL_PATCHES, num_indices, GL_UNSIGNED_INT, 0, num_patches ) ;
    glBindVertexArray(0) ;
}

void Terrain::render( unsigned int shadow_shader_program )
{
    glBindVertexArray(vao) ;
    glDrawElementsInstanced( GL_PATCHES, num_indices, GL_UNSIGNED_INT, 0, num_patches ) ;
    glBindVertexArray(0) ;
}

void Terrain::expand_terrain()
{
    // int s = std::sqrt(num_patches) ;
    if( camera_grid_row < current_i ){
        for( int k = (row*s) ; k < ((row*s)+s) ; k++ )
            models[k] = glm::translate( glm::mat4(1.0f), glm::vec3(models[k][3][0], 0.0f, (index-s/2)*2.0f ) ) ;
        index-- ;
        index2-- ;
        row2 = row ;
        row = (row==0)? s-1 : row - 1 ;
        current_i = camera_grid_row ;
        bind_models() ;
    }
    else if( camera_grid_row > current_i ) {
        for( int k = (row2*s) ; k < ((row2*s)+s) ; k++ )
            models[k] = glm::translate( glm::mat4(1.0f), glm::vec3(models[k][3][0], 0.0f, (index2-s/2)*2.0f ) ) ;
        index2++ ;
        index++ ;
        row = row2 ;
        row2 = (row2 == s-1) ? 0 : row2 + 1 ;
        current_i = camera_grid_row ;
        bind_models() ;
    }
    if( camera_grid_col < current_j ){
        for( int k = col ; k <= ((s-1)*s)+col ; k+=s )
            models[k] = glm::translate( glm::mat4(1.0f), glm::vec3( ((cindex-s/2)*2.0f), 0.0f, models[k][3][2] ) ) ;
        cindex-- ;
        cindex2-- ;
        col2 = col ;
        col = ( col == 0 ) ? s - 1 : col - 1 ;
        current_j = camera_grid_col ;
        bind_models() ;
    }
    else if( camera_grid_col > current_j ) {
        for( int k = col2 ; k <= ((s-1)*s)+col2 ; k+=s )
            models[k] = glm::translate( glm::mat4(1.0f), glm::vec3( ((cindex2-s/2)*2.0f) , 0.0f, models[k][3][2] ) ) ;
        cindex2++ ;
        cindex++ ;
        col = col2 ;
        col2 = ( col2 == s-1 ) ? 0 : col2 + 1 ;
        current_j = camera_grid_col ;
        bind_models() ;
    }
}

void Terrain::activate_textures()
{
    noise->activate_textures() ;
    color->activate_textures() ;
    skybox->activate_irradiance() ;
}

void Terrain::update_time()
{
    float deltaTime = glfwGetTime() - time ;
    time += deltaTime ;
}

void Terrain::update_view( glm::vec3 camera, glm::mat4 v)
{
    int s = std::sqrt(num_patches) ;
    this->camera = camera ;
    glUseProgram( shader_program ) ;
    camera_grid_row = static_cast<int>((camera.z / 2.0f) + (static_cast<float>(s) / 2.0f)) ;
    camera_grid_col = static_cast<int>((camera.x / 2.0f) + (static_cast<float>(s) / 2.0f)) ;
    glUniform3fv( glGetUniformLocation(shader_program, "camera" ), 1, &camera[0] ) ;
    glm::vec3 l( camera.x, 100.0f , camera.z ) ;
    glUniform3fv( glGetUniformLocation(shader_program, "light_position" ), 1, &l[0] ) ;
    glUniformMatrix4fv( glGetUniformLocation( shader_program, "view" ), 1, GL_FALSE, &v[0][0] ) ;
}

void Terrain::generate_error( std::string msg )
{
    std::cout << "\n\033[1;31m" << msg << "\033[0m\n" << std::endl;

    exit(1) ;
}

Terrain::~Terrain()
{
    delete models ;
    models = NULL ;
    delete shader ;
    shader = NULL ;
    glDeleteVertexArrays( 1, &vao ) ;
    glDeleteBuffers( 1, &vbo ) ;
    glDeleteBuffers( 1, &ebo ) ;
}
