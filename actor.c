#include <libdragon.h>
#include <t3d/t3d.h>
#include <t3d/t3dmodel.h>
#include <t3d/t3ddebug.h>

#include "actor.h"



Actor actor_create(
  uint32_t id, 
  rspq_block_t *dpl,
  ActorUpdateFunction actorUpdateFunction,
  ActorDrawFunction actorDrawFunction
)
{
  Actor actor = (Actor){
    .id = id,
    .pos = {0, 25, 50},
    .rot = {0, 0, 0},
    .scale = {1.0, 1.0, 1.0},
    .dpl = dpl,
    .modelMat = malloc_uncached(sizeof(T3DMat4FP)), // needed for t3d
    .updateFunction = actorUpdateFunction,
    .drawFunction = actorDrawFunction,
  };
  t3d_mat4fp_identity(actor.modelMat);
  return actor;
}

void actor_update(Actor *actor, float objTime) {

  actor->updateFunction(actor, objTime);

  // t3d lets you directly construct a fixed-point matrix from SRT
  t3d_mat4fp_from_srt_euler(actor->modelMat, actor->scale, actor->rot, actor->pos);
}

void actor_draw(Actor *actor) {
  t3d_matrix_set(actor->modelMat, true);
  actor->drawFunction(actor);
  rspq_block_run(actor->dpl);
}

void actor_delete(Actor *actor) {
  free_uncached(actor->modelMat);
}
