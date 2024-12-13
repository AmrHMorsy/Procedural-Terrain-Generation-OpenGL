#include "Light.h"




class Camera{
    
private:
    
    GLFWwindow * window ;
    int projection_width, projection_height ;
    glm::vec3 camera_position, front, up, right ;
    glm::mat4 projection, view, model, rotation, lookaround_rotation ;
    float angle, fov, lookaround_speed, near_plane, far_plane ;
    
public:

    Camera( GLFWwindow * _window ) ;
    void look_around( bool rightwards, bool leftwards, bool upwards, bool downwards ) ;
    void set_projection() ;
    void set_view() ;
    void move() ;
    glm::vec3 get_camera_position() ;
    glm::mat4 get_view() ;
    glm::mat4 get_projection() ;
    ~Camera() ;    
} ;
