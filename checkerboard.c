#include <t3d/t3d.h>
#include "actor.h"
#include "checkerboard.h"

void checkerboard_update(Actor *self, float objTime) {
  self->pos[0] = 0;
  self->pos[1] = 0;
  self->pos[2] = 0;
}

void checkerboard_draw(Actor *self) {

}


void checkerboard_square(T3DVertPacked* outVerts, float bottomLeft[2], float topRight[2], uint32_t colour, uint16_t norm) { 
  outVerts[0] = (T3DVertPacked){
    .posA = { bottomLeft[0], 20, bottomLeft[1]}, .rgbaA = colour, .normA = norm,
    .posB = { topRight[0]  , 20, bottomLeft[1]}, .rgbaB = colour, .normB = norm,
  };

  outVerts[1] = (T3DVertPacked){
    .posA = { bottomLeft[0],  20, topRight[1]}, .rgbaA = colour, .normA = norm,
    .posB = { topRight[0],  20, topRight[1]}, .rgbaB = colour, .normB = norm,
  };
} 

Actor checkerboard_create(uint32_t id) { 

  const int TILE_SIZE = 32;
  const int BOARD_SIZE = 8;

  uint16_t norm = t3d_vert_pack_normal(&(T3DVec3){{ 0, 0, 1}}); // normals are packed in a 5.6.5 format
 
  T3DVertPacked* vertices = malloc_uncached(sizeof(T3DVertPacked) * 2);

  // Allocate vertices (make sure to have an uncached pointer before passing it to the API!)
  // For performance reasons, 'T3DVertPacked' contains two vertices at once in one struct.
  vertices[0] = (T3DVertPacked){
    .posA = {-320, 20, 200}, .rgbaA = 0xFF0000'FF, .normA = norm,
    .posB = { 320, 20, 200}, .rgbaB = 0x00FF00'FF, .normB = norm,
  };

  vertices[1] = (T3DVertPacked){
    .posA = { 320,  20, -200}, .rgbaA = 0x0000FF'FF, .normA = norm,
    .posB = {-320,  20, -200}, .rgbaB = 0xFF00FF'FF, .normB = norm,
  };


  // uint32_t black = 0x000000'FF;
  // uint32_t white = 0x00FFFF'FF;

  // uint32_t tileColours[2] = {black, white};

  // for(int i = 0; i < BOARD_SIZE; i++) { 
  //   for(int j = 0; j < BOARD_SIZE; j++) { 
  //     float bottomLeft[2] = {j + j * TILE_SIZE, i + i * TILE_SIZE};
  //     float topRight[2] = {bottomLeft[0] + TILE_SIZE, bottomLeft[1] + TILE_SIZE};
      
  //     checkerboard_square(&vertices[i*j], bottomLeft, topRight, tileColours[j%2], norm);
  //   }
  // }

  rspq_block_t *dplDraw;
  rspq_block_begin();
  
  // for(int i = 0; i < BOARD_SIZE/2; i+=2) { 
  //   for(int j = 0; j < BOARD_SIZE/2; j+=2) { 
  //     t3d_vert_load(&vertices[i*j], 0, 4); // load 4 vertices...
  //       // ...then draw 2 triangles
  //     t3d_tri_draw(0, 1, 2);
  //     t3d_tri_draw(2, 3, 0);
    
  //     t3d_tri_sync(); 
  //   }
  // }
  t3d_vert_load(vertices, 0, 8); // load 4 vertices...
      // ...then draw 2 triangles
    t3d_tri_draw(0, 1, 2);
    t3d_tri_draw(2, 3, 0);


    t3d_tri_draw(4, 5, 6);
    t3d_tri_draw(5, 6, 4);

    t3d_tri_sync(); 

  dplDraw = rspq_block_end();

  Actor  actor = actor_create(id, dplDraw, &checkerboard_update, &checkerboard_draw);

  return actor;
}
