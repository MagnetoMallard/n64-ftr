#ifndef DRAGON_H
#define DRAGON_H

#include <t3d/t3d.h>

static inline void dragon_update(Actor *self, float objTime) {
    self->rot[1] = fm_fmodf(objTime, RAD_360);
}

#endif // DRAGON_H