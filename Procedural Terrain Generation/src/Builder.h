#include "Renderer.h"





class Builder{
    
private:
    
    Renderer * renderer ;
    GLFWwindow * window ;

public:
    
    Builder() ;
    void initialize_GLFW() ;
    void set_window_attributes() ;
    void create_window() ;
    void initialize_GLEW() ;
    void launch_game() ;
    void generate_error( std::string msg ) ;
    ~Builder() ;
    
} ;
