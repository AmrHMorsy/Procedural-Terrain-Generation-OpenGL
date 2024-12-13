#include "Skybox.h"




class Color {
    
private:
    
    GLuint fbo ;
    Shader * shader ;
    GLFWwindow * window ;
    unsigned int vao, vbo, ebo, shader_program, num_indices, model_vbo, num_patches, color_map, resolution ;

public:
    
    Color( GLFWwindow * w, std::vector<float> vertices, std::vector<unsigned int> indices, unsigned int n, glm::mat4 * models, glm::vec3 max, glm::vec3 min ) ;
    void load_shaders() ;
    void generate_buffers() ;
    void bind_vertices( std::vector<float> vertices ) ;
    void bind_models( glm::mat4 * models ) ;
    void bind_indices( std::vector<unsigned int> indices ) ;
    void set_uniform_variables( glm::vec3 max_position, glm::vec3 min_position ) ;
    void create_color_map() ;
    void create_framebuffer() ;
    void bake_noise() ;
    void activate_textures() ;
    void generate_error( std::string msg ) ;
    ~Color() ;

} ;
