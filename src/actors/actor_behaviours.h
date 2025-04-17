// This header contains update functions for actors
// A mix of draw functions and update functions.

#ifndef ACTOR_BEHAVIOURS_H
#define ACTOR_BEHAVIOURS_H

/**
 * @section The Dergs.
 */
static void dragon_update(Actor *self, float objTime) {
    self->rot[1] = fm_fmodf(objTime, RAD_360);
}

/**
 * @section Dynamo.
 */
static surface_t dynamoEyeSurface;
static sprite_t* dynamoEyeTex[4];

static void dynamo_init() {
    dynamoEyeSurface = surface_make_placeholder_linear(1, FMT_RGBA16, 32, 64);
    dynamoEyeTex[0] =  sprite_load("rom:/EYE-DYNAMO1.sprite");
    dynamoEyeTex[1] =  sprite_load("rom:/EYE-DYNAMO-CLOSE.sprite");
    dynamoEyeTex[2] =  sprite_load("rom:/EYE-DYNAMO-DOWN.sprite");
    dynamoEyeTex[3] =  sprite_load("rom:/EYE-DYNAMO-UP.sprite");
}

// ReSharper disable once CppDFAUnreachableFunctionCall
static void dynamo_set_eye_index(int eyeIndex) {
    rdpq_set_lookup_address(1, dynamoEyeTex[eyeIndex]->data);
    rdpq_sync_tile();
    rdpq_mode_tlut(TLUT_NONE);
    rdpq_tex_upload(TILE0, &dynamoEyeSurface, NULL);
}

static void dynamo_part_draw(T3DModelIter *it, T3DModelState *state, float objTime) {
    t3d_model_draw_material(it->object->material, state);
    rspq_block_run(it->object->userBlock);
    it->object->isVisible = false;

    if (strcmp(it->object->material->name, "EYES") ) {
        dynamo_set_eye_index(fm_fmodf(objTime, 3));
    }
}


#endif // ACTOR_BEHAVIOURS_H