#include "Irradiance.h"




class Skybox{

private:

    Shader * skybox ;
    unsigned int resolution ;
    Irradiance * irradiance ;
    Shader * equirect_to_cubemap ;
    unsigned int vao, vbo, skybox_shader, equirectTocubemap_shader , hdr_texture, cubemap, FBO, RBO ;

public:

    Skybox( glm::mat4 view, glm::mat4 projection, glm::vec3 camera ) ;
    void load_hdr_map() ;
    void setup_cubemap() ;
    void create_framebuffers() ;
    void convert_hdrmap_to_cubemap() ;
    void render() ;
    void bind_vertices() ;
    void set_uniform_variables( glm::mat4 view, glm::mat4 projection, glm::vec3 camera ) ;
    void update_view( glm::mat4 view, glm::vec3 camera ) ;
    void activate_texture() ;
    void activate_irradiance() ;
    void activate() ;
    void generate_error( std::string msg ) ;
    ~Skybox() ;

} ;
