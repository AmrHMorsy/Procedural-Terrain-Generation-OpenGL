#include "Color.h"




class Noise {
    
private:
    
    GLuint fbo ;
    Shader * shader ;
    GLFWwindow * window ;
    float max_first_derivative, min_first_derivative, max_second_derivative, min_second_derivative, num_mip_levels ;
    unsigned int vao, vbo, ebo, shader_program, num_indices, model_vbo, num_patches, height_map, resolution ;

public:
    
    Noise( GLFWwindow * w, std::vector<float> vertices, std::vector<unsigned int> indices, unsigned int n, glm::mat4 * models, glm::vec3 max, glm::vec3 min ) ;
    void load_shaders() ;
    void generate_buffers() ;
    void bind_vertices( std::vector<float> vertices ) ;
    void bind_models( glm::mat4 * models ) ;
    void bind_indices( std::vector<unsigned int> indices ) ;
    void set_uniform_variables( glm::vec3 max_position, glm::vec3 min_position ) ;
    void create_height_map() ;
    void create_framebuffer() ;
    void bake_noise() ;
    void activate_textures() ;
    void compute_data() ;
    void compute_num_mip_levels() ;
    float get_num_mip_levels() ;
    float get_min_second_derivative() ;
    float get_max_second_derivative() ;
    float get_min_first_derivative() ;
    float get_max_first_derivative() ;
    void generate_error( std::string msg ) ;
    ~Noise() ;

} ;
