#include <t3d/t3dmath.h>
#include <t3d/t3d.h>
#include "lights.h"

Light light_create(uint8_t colour[4], T3DVec3 direction, bool isPointLight) {
    Light light = {
        .direction = direction,
        .isPointLight = isPointLight
    };
    memcpy(light.colour, colour, sizeof(uint8_t[4]));
    t3d_vec3_norm(&light.direction);
    return light;
}

void light_update(Light* light, uint8_t colour[4], T3DVec3 direction) {
    if (colour != light->colour) memcpy(light->colour, colour, sizeof(uint8_t[4]));
    if (direction.v != light->direction.v) {
        light->direction = direction;
        t3d_vec3_norm(&light->direction);
    }
}

void light_draw(Light* light, int id) {
    if (light->isPointLight) {
        t3d_light_set_point(id, light->colour, &light->direction, 1.0f, false);
    } else {
        t3d_light_set_directional(id, light->colour, &light->direction);
    }
}