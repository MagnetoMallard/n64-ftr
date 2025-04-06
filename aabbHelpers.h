//
// Created by mallard on 06/04/2025.
//

#ifndef AABBHELPERS_H
#define AABBHELPERS_H

#include <libdragon.h>

inline void aabb_translate(int16_t transposedAabb[3], int16_t aabbPoint[3], float pos[3]) {
    transposedAabb[0] = aabbPoint[0] + pos[0];
    transposedAabb[1] = aabbPoint[1] + pos[1];
    transposedAabb[2] = aabbPoint[2] + pos[2];
}


// TODO: Finish This!
inline void aabb_scale(int16_t transposedAabbs[6], int16_t inputAabbs[6],  float scaleFactor[3], float scaleOrigin[3]) {
    // min in 0-2
    // max in 3-5

    transposedAabbs[0] = inputAabbs[0] + scaleOrigin[0];
    transposedAabbs[1] = inputAabbs[1] + scaleOrigin[1];
    transposedAabbs[2] = inputAabbs[2] + scaleOrigin[2];
}

#endif //AABHELPERS_H
