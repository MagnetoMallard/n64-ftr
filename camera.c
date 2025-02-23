#include <libdragon.h>
#include "camera.h"

float rotAngleX = 0.0f;  
float camDist = 20.0f;
float lastTime = 0.0f;

#define RAD_360 6.28318530718f


Camera camera_create() { 
    Camera camera = { 
        .pos = {{0.0f,50.0f,-50.0f}},
        .target = {{0,50,300}}
    };
    return camera;
} 
void camera_draw() { 
    rdpq_text_printf(NULL, FONT_BUILTIN_DEBUG_MONO, 16, 128, "ROT ANGLE X : %.2f",  rotAngleX);

}

void camera_update(_SI_condat *inputs, Camera* camera, T3DViewport* viewport) { 
    rotAngleX = inputs->x * 0.001f;

    T3DVec3 upVector = (T3DVec3){{0,1,0}};

    camera->target.x = camera->pos.x + (camera->target.x-camera->pos.x)*fm_cosf(rotAngleX)-(camera->target.z-camera->pos.z)*fm_sinf(rotAngleX);
    camera->target.z = camera->pos.z + (camera->target.x-camera->pos.x)*fm_sinf(rotAngleX)+(camera->target.z-camera->pos.z)*fm_cosf(rotAngleX);

    T3DVec3 translationVec;
    T3DVec3 normalizedTarget = camera->target;

    t3d_vec3_mul(&translationVec, &(T3DVec3){{inputs->y * 0.1f,0,inputs->y * 0.1f}}, &camera->target);

    t3d_vec3_norm(&translationVec); 
    t3d_vec3_scale(&translationVec, &translationVec, 4.0f);

    t3d_vec3_add(&camera->pos, &camera->pos, &translationVec);
    t3d_vec3_add(&camera->target, &camera->target, &translationVec);

    t3d_viewport_set_projection(viewport, T3D_DEG_TO_RAD(85.0f), 1.0f, 100.0f);
    t3d_viewport_look_at(viewport, &camera->pos, &camera->target, &upVector);
    
    // t3d_mat4_mul(&viewport->matCamProj, &viewport->matProj, &viewport->matCamera);
    // t3d_mat4_to_frustum(&viewport->viewFrustum, &viewport->matCamProj);
    // viewport->_isCamProjDirty = false;
}
