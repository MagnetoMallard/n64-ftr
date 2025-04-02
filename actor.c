#include <t3d/t3dmodel.h>
#include <t3d/t3dskeleton.h>

#include "actor.h"
#include "globals.h"


Actor actor_create(
  rspq_block_t *dpl,
  ActorUpdateFunction actorUpdateFunction,
  ActorDrawFunction actorDrawFunction
)
{
  Actor actor = (Actor){
    .pos = {0, 25, 50},
    .rot = {0, 0, 0},
    .scale = {1.0, 1.0, 1.0},
    .dpl = dpl,
    .modelMat = malloc_uncached(sizeof(T3DMat4FP)), // needed for t3d
    .updateFunction = actorUpdateFunction,
    .drawFunction = actorDrawFunction,
    .anim = {}
  };
  t3d_mat4fp_identity(actor.modelMat);
  return actor;
}

void actor_update(Actor *actor, float objTime) {
  if(actor->updateFunction) { 
    actor->updateFunction(actor, objTime);
  }

  if (actor->anim.animationCount) {
    t3d_anim_update(&actor->anim.animationInstances[1], objTime);
  }
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
  animations_teardown(actor->anim);
}

Actor create_actor_from_model(char* modelName) {

  Actor actor = actor_create(NULL, NULL, NULL);

  // Load a model-file, this contains the geometry and some metadata
  // Then cache it into a display list
  char filename[32];
  sprintf(filename, "rom:/%s.t3dm", modelName);
  T3DModel *actorModel = t3d_model_load(filename);

  // Detect if model has animations
  // And add them to the struct if they do
  uint32_t animationCount = t3d_model_get_animation_count(actorModel);
  debugf("Model name, %s\n", filename);
  debugf("Animation count: %ld\n", animationCount);
  if (animationCount) {
    actor.anim =  create_from_model(actorModel, animationCount);
  }

  if (animationCount) {
    rspq_block_begin();
      t3d_model_draw_skinned(actorModel, &actor.anim.skel);
    actor.dpl = rspq_block_end();
  } else {
    rspq_block_begin();
      t3d_model_draw(actorModel);
    actor.dpl = rspq_block_end();
  }


  return actor;
}