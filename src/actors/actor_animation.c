//
// Created by mallard on 27/03/2025.
//
#include <t3d/t3dmodel.h>
#include <t3d/t3danim.h>

#include "actor_animation.h"

ActorAnimation create_from_model(const T3DModel* model, uint32_t animationCount) {
    ActorAnimation animationStruct = (ActorAnimation){
        .animationCount = animationCount,
        .animations = NULL,
        .animationInstances = NULL,
        .skel = t3d_skeleton_create(model),

    };

    debugf("model pointer %p\n", model);
    animationStruct.animations = malloc(animationCount * sizeof(void*));


    t3d_model_get_animations(model, animationStruct.animations);
    animationStruct.animationInstances = malloc(animationCount * sizeof(T3DAnim));

    for(int j = 0; j < t3d_model_get_animation_count(model); j++) {
        animationStruct.animationInstances[j] =
            t3d_anim_create(model, t3d_model_get_animation(model, animationStruct.animations[j]->name)->name);
        t3d_anim_attach(&animationStruct.animationInstances[j], &animationStruct.skel);
    }
    T3DAnim* animInst = &animationStruct.animationInstances[1];
    t3d_anim_set_time(animInst, 0.0f);
    t3d_anim_set_speed(animInst, 1.0f);
    t3d_anim_set_playing(animInst, 1);
    t3d_anim_set_looping(animInst, 1);
    debugf("Animation name: %s\n", animInst->animRef->name);
    debugf("Animation duration: %.2f\n", animInst->animRef->duration);
    return animationStruct;
}

void animations_teardown(ActorAnimation animationStruct) {
    free_uncached(animationStruct.animationInstances);
    free_uncached(animationStruct.animations);
}

void animations_update(ActorAnimation animationStruct) {
    free_uncached(animationStruct.animationInstances);
    free_uncached(animationStruct.animations);
}
