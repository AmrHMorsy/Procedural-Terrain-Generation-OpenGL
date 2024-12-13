#include "Camera.h"




class Renderer{
    
private:
    
    Light * light ;
    Shadow * shadow ;
    Skybox * skybox ;
    Camera * camera ;
    bool key_pressed ;
    Terrain * terrain ;
    GLFWwindow * window ;
    int counter, fps_avg, fps_max, fps_min ;
    float avg_time, time, max_time, min_time ;

public:
    
    Renderer( GLFWwindow * window_ ) ;
    void set() ;
    void start_game_loop() ;
    void render_shadow() ;
    void render_scene() ;
    void handle_events() ;
    void compute_fps() ; 
    void update_view() ;
    void generate_error( std::string error_msg ) ;
    ~Renderer() ;
} ;
