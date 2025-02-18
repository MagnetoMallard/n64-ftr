#include <t3d/t3d.h>
#include "actor.h"
#include "checkerboard.h"

void checkerboard_update(Actor *self, float objTime) {
  self->pos[0] = 0;
  self->pos[1] = 0;
  self->pos[2] = 0;
}

void checkerboard_draw(Actor *self) {

}

Actor checkerboard_create(uint32_t id) { 

  uint16_t norm = t3d_vert_pack_normal(&(T3DVec3){{ 0, 0, 1}}); // normals are packed in a 5.6.5 format
  T3DVertPacked* vertices = malloc_uncached(sizeof(T3DVertPacked) * 2);

  // Allocate vertices (make sure to have an uncached pointer before passing it to the API!)
  // For performance reasons, 'T3DVertPacked' contains two vertices at once in one struct.
  vertices[0] = (T3DVertPacked){
    .posA = {-320, 16, 132}, .rgbaA = 0xFF0000'FF, .normA = norm,
    .posB = { 320, 16, 132}, .rgbaB = 0x00FF00'FF, .normB = norm,
  };

  vertices[1] = (T3DVertPacked){
    .posA = { 320,  16, 0}, .rgbaA = 0x0000FF'FF, .normA = norm,
    .posB = {-320,  16, 0}, .rgbaB = 0xFF00FF'FF, .normB = norm,
  };

  rspq_block_t *dplDraw;
  rspq_block_begin();

  t3d_vert_load(vertices, 0, 4); // load 4 vertices...
    // ...then draw 2 triangles
  t3d_tri_draw(0, 1, 2);
  t3d_tri_draw(2, 3, 0);

  t3d_tri_sync(); 

  dplDraw = rspq_block_end();

  Actor  actor = actor_create(id, dplDraw, &checkerboard_update, &checkerboard_draw);

  return actor;
}