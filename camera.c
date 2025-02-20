#include <libdragon.h>
#include "camera.h"

float rotAngle = 0.0f;

Camera camera_create() { 
    Camera camera = { 
        .pos = {{0.0f,50.0f,175.0f}},
        .target = {{0,50,0}}
    };
    return camera;
} 

void camera_update(_SI_condat *inputs, Camera* camera, T3DViewport* viewport) { 
    rotAngle -= (inputs->x);
    camera->target.x = -(rotAngle*0.1);
    camera->target.z = (cos(rotAngle*0.1));
    t3d_viewport_set_projection(viewport, T3D_DEG_TO_RAD(80.0f), 1.0f, 100.0f);
    t3d_viewport_look_at(viewport, &camera->pos, &camera->target, &(T3DVec3){{0,1,0}});
}
