//
// Created by mallard on 27/03/2025.
//
#ifndef ACTOR_ANIMATED_H
#define ACTOR_ANIMATED_H

#include <t3d/t3d.h>
#include <t3d/t3dmodel.h>
#include <t3d/t3danim.h>

struct AnimatedActor;

typedef struct _animatedActor {
    uint32_t animationCount;
    T3DChunkAnim **animations;
    T3DAnim *animationInstances;
    T3DSkeleton skel;
} AnimatedActor;

AnimatedActor create_from_model(const T3DModel* model, uint32_t animationCount);
void animations_teardown(AnimatedActor animationStruct);

#endif //ACTOR_ANIMATED_H
