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


static void aabb_mat4_mult(int16_t transposedAabbs[3],int16_t initialAabbs[3], T3DMat4* multMatrix) {
    T3DVec3 vec = {{initialAabbs[0], initialAabbs[1],  initialAabbs[2]  }};

    T3DVec4 outputVec;
    t3d_mat4_mul_vec3(&outputVec, multMatrix, &vec);

    transposedAabbs[0] = outputVec.x;
    transposedAabbs[1] = outputVec.y;
    transposedAabbs[2] = outputVec.z;
}

#endif //AABHELPERS_H


