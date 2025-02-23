#ifndef CAMERA_H
#define CAMERA_H

#include <libdragon.h>
#include <t3d/t3d.h>
#include <t3d/t3dmath.h>
#include <t3d/t3dmodel.h>

typedef struct { 
    T3DVec3 pos;
    T3DVec3 target;
} Camera;

Camera camera_create();
void camera_update(_SI_condat *inputs, Camera* camera, T3DViewport* viewport);
void camera_draw();

#endif