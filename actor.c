#include <libdragon.h>
#include <t3d/t3d.h>
#include <t3d/t3dmodel.h>
#include <t3d/t3ddebug.h>

#include "actor.h"

#define RAD_360 6.28318530718f


Actor actor_create(uint32_t id, rspq_block_t *dpl)
{
  Actor actor = (Actor){
    .id = id,
    .pos = {0, 25, 50},
    .rot = {0, 0, 0},
    .scale = {1.0, 1.0, 1.0},
    .dpl = dpl,
    .modelMat = malloc_uncached(sizeof(T3DMat4FP)) // needed for t3d
  };
  t3d_mat4fp_identity(actor.modelMat);
  return actor;
}

void actor_update(Actor *actor, float objTime) {
  actor->pos[0] = 0;

  // set some random position and rotation
  float randRot = (float)fm_fmodf(actor->id * 123.1f, 5.0f);
  float randDist = (float)fm_fmodf(actor->id * 4645.987f, 30.5f) + 10.0f;

  //actor->rot[0] = fm_fmodf(randRot + objTime * 1.05f, RAD_360);
  actor->rot[1] = fm_fmodf(objTime, RAD_360);
 // actor->rot[2] = fm_fmodf(randRot + objTime * 1.1f, RAD_360);

  // actor->pos[0] = randDist * fm_cosf(objTime * 1.6f + randDist);
  // actor->pos[1] = randDist * fm_sinf(objTime * 1.5f + randRot);
  // actor->pos[2] = randDist * fm_cosf(objTime * 1.4f + randDist*randRot);

  // t3d lets you directly construct a fixed-point matrix from SRT
  t3d_mat4fp_from_srt_euler(actor->modelMat, actor->scale, actor->rot, actor->pos);
}

void actor_draw(Actor *actor) {
  t3d_matrix_set(actor->modelMat, true);
  rspq_block_run(actor->dpl);
}

void actor_delete(Actor *actor) {
  free_uncached(actor->modelMat);
}
