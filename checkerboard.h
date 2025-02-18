#ifndef CHECKERBOARD_H
#define CHECKERBOARD_H

#include <t3d/t3d.h>
#include "actor.h"

typedef struct {
  Actor actor;
  rspq_block_t* dpl;
  T3DVertPacked* vertices;
} Checkerboard;

Checkerboard create_checkerboard(uint32_t id);

#endif // CHECKERBOARD_H