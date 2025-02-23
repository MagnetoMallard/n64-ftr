#include <libdragon.h>
#include "camera.h"

float rotAngleX = 0.0f;  
float rotAngleY = 0.0f;  
float camDist = 20.0f;
float lastTimeMs = 0.0f;
float time = 0.0f;


T3DVec3 camDir = {{0,0,1}};

#define RAD_360 6.28318530718f


Camera camera_create() { 
    Camera camera = { 
        .pos = {{0.0f,50.0f,-50.0f}},
        .target = {{0,50,50}}
    };
    return camera;
} 
void camera_draw() { 
    rdpq_text_printf(NULL, FONT_BUILTIN_DEBUG_MONO, 16, 128, "ROT ANGLE X : %.2f",  rotAngleX);

}

void camera_update(_SI_condat *inputs, Camera* camera, T3DViewport* viewport) { 
    T3DVec3 upVector = (T3DVec3){{0,1,0}};
    
    double nowMs = (double)get_ticks_us() / 1000.0;
    float deltaTime = (float)(nowMs - lastTimeMs);
    lastTimeMs = nowMs;
    time += deltaTime;

    float camSpeed = deltaTime * 0.001f;
    float camRotSpeed = deltaTime * 0.00001f;

    camDir.v[0] = fm_cosf(rotAngleX) * fm_cosf(rotAngleY);
    camDir.v[1] = fm_sinf(rotAngleY);
    camDir.v[2] = fm_sinf(rotAngleX) * fm_cosf(rotAngleY);
    t3d_vec3_norm(&camDir);

    if(inputs->Z) {
      rotAngleX += (float)inputs->x * camRotSpeed;
      rotAngleY += (float)inputs->y * camRotSpeed;
    } else {
      camera->pos.v[0] += camDir.v[0] * (float)inputs->y * camSpeed;
      camera->pos.v[1] += camDir.v[1] * (float)inputs->y * camSpeed;
      camera->pos.v[2] += camDir.v[2] * (float)inputs->y * camSpeed;

      camera->pos.v[0] += camDir.v[2] * (float)inputs->x * -camSpeed;
      camera->pos.v[2] -= camDir.v[0] * (float)inputs->x * -camSpeed;
    }

    if(inputs->C_up)camera->pos.v[1] += camSpeed * 15.0f;
    if(inputs->C_down)camera->pos.v[1] -= camSpeed * 15.0f;

    camera->target.v[0] = camera->pos.v[0] + camDir.v[0];
    camera->target.v[1] = camera->pos.v[1] + camDir.v[1];
    camera->target.v[2] = camera->pos.v[2] + camDir.v[2];

    t3d_viewport_set_projection(viewport, T3D_DEG_TO_RAD(85.0f), 1.0f, 300.0f);
    t3d_viewport_look_at(viewport, &camera->pos, &camera->target, &upVector);
    
    // t3d_mat4_mul(&viewport->matCamProj, &viewport->matProj, &viewport->matCamera);
    // t3d_mat4_to_frustum(&viewport->viewFrustum, &viewport->matCamProj);
    // viewport->_isCamProjDirty = false;
}
