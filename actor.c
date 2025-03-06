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

inline void actor_draw(Actor *actor) {
  t3d_matrix_push_pos(1);
  t3d_matrix_set(actor->modelMat, true);
 // actor->drawFunction(actor);
  rspq_block_run(actor->dpl);
  t3d_matrix_pop(1);
}

void actor_delete(Actor *actor) {
  free_uncached(actor->modelMat);
}

Actor create_actor_from_model(uint32_t id, char* modelName) { 

  uint16_t norm = t3d_vert_pack_normal(&(T3DVec3){{ 0, 0, 1}}); // normals are packed in a 5.6.5 format
  T3DVertPacked* vertices = malloc_uncached(sizeof(T3DVertPacked) * 2);
  // Load a model-file, this contains the geometry and some metadata
  // Then cache it into a display list
  char filename[32];
  sprintf(filename, "rom:/%s.t3dm", modelName);
  T3DModel *dragonModel = t3d_model_load(filename);
  rspq_block_t *dragonDpl;

  rspq_block_begin();
  t3d_model_draw(dragonModel);
  dragonDpl = rspq_block_end();

  Actor  actor = actor_create(id, dragonDpl, NULL, NULL);

  return actor;
}