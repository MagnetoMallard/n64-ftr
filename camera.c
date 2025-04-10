#include <libdragon.h>
#include "camera.h"
#include "globals.h"
#include "../tiny3d/src/t3d/t3d.h"

float rotAngleX = 0.0f;
float rotAngleY = 0.0f;
float rotAngleZ = 0.0f;
float camDist = 20.0f;
float lastTimeMs = 0.0f;

T3DVec3 camDir = {{0,0,1}};
T3DVec3 upVector = (T3DVec3){{0,1,0}};

T3DVec3 dergVector;
T3DVec3 dynamoVector;

extern
#define RAD_360 6.28318530718f
#define STICk_RANGE_X (RAD_360 / 2)
#define STICk_RANGE_Y (RAD_360 / 2)
Camera camera_create() {
    Camera camera = { 
        .pos = {{0.0f,50.0f,-50.0f}},
        .target = {{0,50,50}},
        .rotation = {{0.0f,0.0f,0.0f}}
    };
    return camera;
} 
void camera_draw() { 
    // rdpq_text_printf(NULL, FONT_BUILTIN_DEBUG_MONO, 16, 128, "ROT ANGLE X : %.2f",  rotAngleX);
}

void camera_look_at(Camera* camera, T3DVec3 *target, T3DViewport* viewport) {
    T3DVec3 dirVec;
    t3d_vec3_diff(&dirVec,target, &camera->pos);
    t3d_vec3_norm(&dirVec);

    camera->target = *target;
    debugf("rotVec: %.2f, %.2f, %.2f\n", camera->rotation.x, camera->rotation.y, camera->rotation.z);

    debugf("dirVec: %.2f, %.2f, %.2f\n", dirVec.x, dirVec.y, dirVec.z);

    camera->rotation.x = fm_atan2f(dirVec.z, dirVec.x);
     //camera->rotation.y = fm_atan2f(dirVec.y, dirVec.x) / 2;

}


void camera_update(Camera* camera, T3DViewport* viewport, float deltaTime) {

    float camSpeed = deltaTime * 2.0f;
    float camRotSpeed = deltaTime * 0.02f;

    camDir.v[0] =  fm_cosf(camera->rotation.x) * fm_cosf(camera->rotation.y);
    camDir.v[1] =  fm_sinf(camera->rotation.y);
    camDir.v[2] =  fm_sinf(camera->rotation.x) * fm_cosf(camera->rotation.y);

    bool inStickRangeY = camera->rotation.y <= STICk_RANGE_Y && camera->rotation.y >= -STICk_RANGE_Y;

    if(inputs.btn.z) {
      if (inputs.stick_y && inStickRangeY) {
          float rotY =  camera->rotation.y + (float)inputs.stick_y * camRotSpeed;
          if (rotY <= STICk_RANGE_Y && rotY >= -STICk_RANGE_Y) {
              camera->rotation.y = rotY;
          }
      }
      camera->pos.v[0] += camDir.v[2] * (float)inputs.stick_y * -camSpeed;
      camera->pos.v[2] -= camDir.v[0] * (float)inputs.stick_x * -camSpeed;
    } else {
      camera->pos.v[0] += camDir.v[0] * (float)inputs.stick_y * camSpeed;
      camera->pos.v[1] += camDir.v[1] * (float)inputs.stick_y * camSpeed;
      camera->pos.v[2] += camDir.v[2] * (float)inputs.stick_y * camSpeed;
      if (inputs.stick_x) camera->rotation.x += (float)inputs.stick_x * camRotSpeed;
    }

      camera->rotation.x = fm_fmodf(camera->rotation.x, RAD_360);

    if(inputs.btn.c_up)camera->pos.v[1] += camSpeed * 15.0f;
    if(inputs.btn.c_down)camera->pos.v[1] -= camSpeed * 15.0f;
    if(btnsPressed.a)    camera_look_at(camera,&dergVector,viewport);
    if(btnsPressed.b)    camera_look_at(camera,&dynamoVector,viewport);

    camera->target.v[0] = camera->pos.v[0] + camDir.v[0];
    camera->target.v[1] = camera->pos.v[1] + camDir.v[1];
    camera->target.v[2] = camera->pos.v[2] + camDir.v[2];

    t3d_viewport_set_projection(viewport, T3D_DEG_TO_RAD(85.0f), 10.0f, 250.0f);
    t3d_viewport_look_at(viewport, &camera->pos, &camera->target, &upVector);
}
