#ifndef ACTOR_H
#define ACTOR_H

static float objTimeLast = 0.0f;
static float baseSpeed = 1.0f;

// Holds our actor data, relevant for t3d is 'modelMat'.
typedef struct {
  uint32_t id;
  float pos[3];
  float rot[3];
  float scale[3];

  rspq_block_t *dpl;
  T3DMat4FP *modelMat;
} Actor;

Actor actor_create(uint32_t id, rspq_block_t *dpl);
void actor_update(Actor *actor, float objTime);
void actor_draw(Actor *actor);
void actor_delete(Actor *actor);

#endif