#ifndef ACTOR_H
#define ACTOR_H

#include <libdragon.h>
#include <t3d/t3d.h>

#define RAD_360 6.28318530718f

static float objTimeLast = 0.0f;
static float baseSpeed = 1.0f;

struct Actor;

// Holds our actor data, relevant for t3d is 'modelMat'.
typedef struct _actor {
  float pos[3];
  float rot[3];
  float scale[3];
  rspq_block_t *dpl;
  T3DMat4FP *modelMat;
  void (*updateFunction)(struct _actor*,float);
  void (*drawFunction)(struct _actor*);
} Actor;

typedef void (*ActorUpdateFunction)(Actor*,float);
typedef void (*ActorDrawFunction)(Actor*);

Actor actor_create(
  rspq_block_t *dpl,
  ActorUpdateFunction actorUpdateFunction, ActorDrawFunction actorDrawFunction
); 
void actor_update(Actor *actor, float objTime);
void actor_draw(Actor *actor);
void actor_delete(Actor *actor);
Actor create_actor_from_model(char* modelName); 

#endif