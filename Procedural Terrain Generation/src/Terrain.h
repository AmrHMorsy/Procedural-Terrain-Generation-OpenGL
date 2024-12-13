#include "Noise.h"




class Terrain {
    
private:
    
    int level ; 
    float max_x ;
    float max_z ;
    float min_x ;
    float min_z ;

    
    float time ;
    int max_lod ; 
    Noise * noise ;
    Skybox * skybox ;
    Shader * shader ;
    int sqrt_num_patches ;
    std::vector<float> vertices ;
    std::vector<unsigned int> indices ;
    glm::mat4 * models, projection, view, light_projection, light_view ;
    glm::vec3 light_color, camera, light_position, max_position, min_position ;
    unsigned int vao, vbo, ebo, shader_program, num_indices, model_vbo, num_patches ;
    unsigned int row, row2 ;
    int index, current_i, index2 ;
    unsigned int col, col2 ;
    int cindex, current_j, cindex2 ;
    glm::vec3 level_min_boundary, level_max_boundary ; 
    float min_second_derivative, max_second_derivative ; 
    float min_first_derivative, max_first_derivative ; 
    Color * color ;
    int s ;
    int camera_grid_row ;
    int camera_grid_col ;


public:
    
    Terrain( GLFWwindow * w, glm::mat4 v, glm::mat4 proj, glm::vec3 c, glm::mat4 lightview, glm::mat4 lightproj, glm::vec3 lightpos, glm::vec3 lightcol ) ;
    void load_shaders() ;
    void set( Skybox * skybox ) ;
    void generate_buffers() ;
    void render( Shadow * shadow ) ;
    void render( unsigned int shadow_shader_program ) ;
    void bind_vertices( std::vector<float> vertices ) ;
    void bind_models() ;
    void update_time() ;
    void bind_indices( std::vector<unsigned int> indices ) ;
    void set_uniform_variables() ;
    void set_models() ;
    void update_view( glm::vec3 camera, glm::mat4 v ) ;
    void expand_terrain() ; 
    void update_noise_level() ;
    void activate_textures() ;
    void generate_error( std::string msg ) ;
    ~Terrain() ;

} ;
