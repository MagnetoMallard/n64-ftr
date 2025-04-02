#include <t3d/t3d.h>
#include "actor.h"
#include <t3d/t3dmodel.h>

void dragon_update(Actor *self, float objTime) {
    self->rot[1] = fm_fmodf(objTime, RAD_360);
}

void dragon_draw(Actor *self) {

}

Actor dragon_create(void) {

  uint16_t norm = t3d_vert_pack_normal(&(T3DVec3){{ 0, 0, 1}}); // normals are packed in a 5.6.5 format
  // T3DVertPacked* vertices = malloc_uncached(sizeof(T3DVertPacked) * 2);
  // Load a model-file, this contains the geometry and some metadata
  // Then cache it into a display list
  T3DModel *dragonModel = t3d_model_load("rom:/dragon2.t3dm");
  rspq_block_t *dragonDpl;

  rspq_block_begin();
  t3d_model_draw(dragonModel);
  dragonDpl = rspq_block_end();

  Actor  actor = actor_create(dragonDpl, &dragon_update, &dragon_draw);

  return actor;
}