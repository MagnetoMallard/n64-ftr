//
// Created by mallard on 27/03/2025.
//
#ifndef ACTOR_ANIMATED_H
#define ACTOR_ANIMATED_H

#include <t3d/t3d.h>
#include <t3d/t3dmodel.h>
#include <t3d/t3danim.h>

struct ActorAnimation;

typedef struct _animatedActor {
    uint32_t animationCount;
    T3DChunkAnim **animations;
    T3DAnim *animationInstances;
    T3DSkeleton skel;
    int currentAnimation;
} ActorAnimation;

ActorAnimation create_from_model(const T3DModel* model, uint32_t animationCount);
void animations_teardown(ActorAnimation animationStruct);
void animations_change(ActorAnimation* animationStruct, int number, float speed);

#endif //ACTOR_ANIMATED_H
