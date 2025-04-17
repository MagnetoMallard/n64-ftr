#ifndef ACTOR_H
#define ACTOR_H


#include <libdragon.h>
#include <t3d/t3d.h>

#include "actor_animation.h"

#define RAD_360 6.28318530718f

struct Actor;

// Holds our actor data, relevant for t3d is 'modelMat'.
typedef struct _actor {
  char* name;
  float pos[3];
  float rot[3];
  float scale[3];
  bool visible;
  int animCount;
  int16_t initialAabbMin[3];
  int16_t initialAabbMax[3];
  rspq_block_t *dpl;
  T3DMat4FP *modelMat;
  T3DMat4 *modelMatF;
  ActorAnimation anim;
  T3DModel *t3dModel;
  void (*updateFunction)(struct _actor*,float);
  void (*drawFunction)(struct _actor*);
  void (*customPartDrawFunc)(T3DModelIter*, T3DModelState*, float);
} Actor;

typedef void (*ActorUpdateFunction)(Actor*,float);
typedef void (*ActorDrawFunction)(Actor*);

Actor actor_create(
  rspq_block_t *dpl,
  ActorUpdateFunction actorUpdateFunction, ActorDrawFunction actorDrawFunction
); 
void actor_update(Actor *actor, float objTime, float deltaTime);
void actor_draw(Actor *actor, float objTime);
void actor_delete(Actor *actor);
void actor_attach_update_function(Actor* actor, ActorUpdateFunction updateFunction);

Actor create_actor_from_model(char* modelName);

inline T3DVec3 actor_get_pos_vec(Actor *actor) {
  return (T3DVec3){{actor->pos[0],actor->pos[1],actor->pos[2]}};
}

inline T3DVec3 actor_get_rot_vec(Actor *actor) {
  return (T3DVec3){{actor->rot[0],actor->rot[1],actor->rot[2]}};
}

inline T3DVec3 actor_get_scale_vec(Actor *actor) {
  return (T3DVec3){{actor->scale[0],actor->scale[1],actor->scale[2]}};
}

#endif