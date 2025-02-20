#include <t3d/t3dmath.h>
#include <t3d/t3d.h>
#include "lights.h"

Light light_create(uint8_t colour[4], T3DVec3 direction) { 
    Light light = {
        .colour = colour,
        .direction = direction
    };
    t3d_vec3_norm(&light.direction);
    return light;
}

void light_update(Light* light, uint8_t colour[4], T3DVec3 direction) {
    light->colour = colour;
    light->direction = direction;
    t3d_vec3_norm(&light->direction);
}

void light_draw(Light* light, int id) { 
    t3d_light_set_directional(id, light->colour, &light->direction);
}