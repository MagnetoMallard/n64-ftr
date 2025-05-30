#ifndef LIGHTS_H
#define LIGHTS_H

#include <t3d/t3dmath.h>

typedef struct _light {
    uint8_t colour[4];
    T3DVec3 direction;
    bool isPointLight;
    void (*lightUpdateFunction)(struct _light*, float, float);
} Light;

Light light_create(uint8_t colour[4], T3DVec3 direction, bool isPointLight);
void light_update(Light* light, uint8_t colour[4], T3DVec3 direction, float objTime);
void light_draw(Light* light, int id);

typedef void (*LightUpdateFunction)(struct _light*, float, float);

#endif