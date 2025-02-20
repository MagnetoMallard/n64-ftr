#ifndef LIGHTS_H
#define LIGHTS_H

#include <t3d/t3dmath.h>

typedef struct _light {
    uint8_t* colour;
    T3DVec3 direction;
} Light;

Light light_create(uint8_t colour[4], T3DVec3 direction);
void light_update(Light* light, uint8_t colour[4], T3DVec3 direction);
void light_draw(Light* light, int id);

#endif