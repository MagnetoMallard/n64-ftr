#include <t3d/t3dmodel.h>
#include <t3d/t3dskeleton.h>
#include "../globals.h"

// Helpers
#include "../helpers/debug_draw.h"
#include "../helpers/aabb_helpers.h"

#include "actor.h"

Actor actor_create(
    rspq_block_t *dpl,
    ActorUpdateFunction actorUpdateFunction,
    ActorDrawFunction actorDrawFunction
) {
    Actor actor = (Actor){
        .pos = {0, 0, 0},//0,25,50
        .rot = {0, 0, 0},
        .scale = {1.0, 1.0, 1.0},
        .visible = true,
        .animCount = 0,
        .dpl = dpl,
        .modelMat = malloc_uncached(sizeof(T3DMat4FP)), // needed for t3d
        .modelMatF = malloc_uncached(sizeof(T3DMat4)), // needed for t3d
        .updateFunction = actorUpdateFunction,
        .drawFunction = actorDrawFunction,
        .anim = {},
        .t3dModel = malloc_uncached(sizeof(T3DModel))
    };
    t3d_mat4fp_identity(actor.modelMat);
    return actor;
}

static void actor_recalc_aabs(Actor *actor) {
    aabb_mat4_mult(actor->t3dModel->aabbMin, actor->initialAabbMin, actor->modelMatF);
    aabb_mat4_mult(actor->t3dModel->aabbMax, actor->initialAabbMax, actor->modelMatF);
}

void actor_update(Actor *actor, float objTime, float deltaTime) {

    if (actor->updateFunction) {
        actor->updateFunction(actor, objTime);
    }

    if (actor->anim.animationCount) {
        t3d_anim_update(&actor->anim.animationInstances[actor->anim.currentAnimation], deltaTime );
    }

    t3d_mat4_from_srt_euler(actor->modelMatF, actor->scale, actor->rot,actor->pos );
    t3d_mat4_to_fixed_3x4(actor->modelMat, actor->modelMatF);
    actor_recalc_aabs(actor);
}


inline void actor_draw(Actor *actor, float objTime) {
    t3d_matrix_set(actor->modelMat, true);

    if (actor->visible) {
        T3DModelState state = t3d_model_state_create();
        T3DModelIter it = t3d_model_iter_create(actor->t3dModel, T3D_CHUNK_TYPE_OBJECT);
        while (t3d_model_iter_next(&it)) {
            if (it.object->isVisible) {
                // draw material and object
                if (actor->customPartDrawFunc) {
                    actor->customPartDrawFunc(&it, &state, objTime);
                } else {
                    t3d_model_draw_material(it.object->material, &state);
                    rspq_block_run(it.object->userBlock);
                    it.object->isVisible = false; // BVH only sets visible objects, so we need to reset this
                }

            }
        }
    }
}

void actor_delete(Actor *actor) {
    free_uncached(actor->modelMat);
    free_uncached(actor->t3dModel);
    animations_teardown(actor->anim);
}

void actor_attach_update_function(Actor *actor, ActorUpdateFunction updateFunction) {
    actor->updateFunction = updateFunction;
}

Actor create_actor_from_model(char *modelName) {
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
    debugf("AABBmin x: %i\n", actorModel->aabbMin[0]);
    debugf("AABBmin y: %i\n", actorModel->aabbMin[1]);
    debugf("AABBmin z: %i\n", actorModel->aabbMin[2]);

    debugf("AABBmax x: %i\n", actorModel->aabbMax[0]);
    debugf("AABBmax y: %i\n", actorModel->aabbMax[1]);
    debugf("AABBmax z: %i\n", actorModel->aabbMax[2]);

    if (animationCount) {
        actor.anim =  create_from_model(actorModel, animationCount);
    }

    T3DModelIter it = t3d_model_iter_create(actorModel, T3D_CHUNK_TYPE_OBJECT);
    while (t3d_model_iter_next(&it)) {
        rspq_block_begin();
        debugf("part name: %s\n", it.object->name);
        debugf("material name: %s\n", it.object->material->name);

        if (animationCount) {
            t3d_model_draw_object(it.object, actor.anim.skel.boneMatricesFP);
        } else {
            t3d_model_draw_object(it.object, nullptr);
        }
        it.object->userBlock = rspq_block_end();
    }

    memcpy(actor.initialAabbMin, actorModel->aabbMin, sizeof(int16_t[3]));
    memcpy(actor.initialAabbMax, actorModel->aabbMax, sizeof(int16_t[3]));

    actor.name = modelName;
    actor.t3dModel = actorModel;
    actor.animCount = animationCount;

    return actor;
}
