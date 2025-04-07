#ifndef ACTOR_H
#define ACTOR_H


#include <libdragon.h>
#include <t3d/t3d.h>

#include "actor_animated.h"
#define RAD_360 6.28318530718f

static float objTimeLast = 0.0f;
static float baseSpeed = 1.0f;

struct Actor;

// Holds our actor data, relevant for t3d is 'modelMat'.
typedef struct _actor {
  char* name;
  float pos[3];
  float rot[3];
  float scale[3];
  bool visible;
  int16_t initialAabbMin[3];
  int16_t initialAabbMax[3];
  rspq_block_t *dpl;
  T3DMat4FP *modelMat;
  AnimatedActor anim;
  T3DModel *t3dModel;
  void (*updateFunction)(struct _actor*,float);
  void (*drawFunction)(struct _actor*);
} Actor;

typedef void (*ActorUpdateFunction)(Actor*,float);
typedef void (*ActorDrawFunction)(Actor*);

Actor actor_create(
  rspq_block_t *dpl,
  ActorUpdateFunction actorUpdateFunction, ActorDrawFunction actorDrawFunction
); 
void actor_update(Actor *actor, float objTime, float deltaTime);
void actor_draw(Actor *actor);
void actor_delete(Actor *actor);
void actor_attach_update_function(Actor* actor, ActorUpdateFunction updateFunction);
Actor create_actor_from_model(char* modelName); 

#endif