#ifndef CAMERA_H
#define CAMERA_H

#include <libdragon.h>
#include <t3d/t3d.h>
#include <t3d/t3dmath.h>

typedef struct { 
    T3DVec3 pos;
    T3DVec3 target;
    T3DVec3 rotation;
    bool inputEnabled;
} Camera;

Camera camera_create();
void camera_look_at(Camera* camera, T3DVec3 *target, T3DViewport* viewport);
void camera_update(Camera* camera, T3DViewport* viewport, float objTime);
#endif