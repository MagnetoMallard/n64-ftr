#ifndef ACTOR_BEHAVIOURS_H
#define ACTOR_BEHAVIOURS_H

#include <t3d/t3d.h>
#include "globals.h"


static inline void dragon_update(Actor *self, float objTime) {
    self->rot[1] = fm_fmodf(objTime, RAD_360);
}

static inline void dynamo_part_draw(T3DModelIter *it, T3DModelState *state, float objTime) {
    char strCheck[12]  = "Eyebrow.R";
    if ( it->object->name == strCheck ) {
        int objTimeInt = objTime;
        // if (objTimeInt % 10) {
        //     // sets up tex manuallly
        //     rdpq_set_lookup_address(1, dynamoEyeTex[1]->data);
        // } else {
        //     t3d_model_draw_material(it->object->material, state);
        // }
        debugf("doin it!");
        rdpq_set_lookup_address(1, dynamoEyeTex[1]->data);

    }
    t3d_model_draw_material(it->object->material, state);
    rspq_block_run(it->object->userBlock);
    it->object->isVisible = false;
}


#endif // ACTOR_BEHAVIOURS_H