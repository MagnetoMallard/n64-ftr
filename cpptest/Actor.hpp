#ifndef _ACTOR_HPP
#define _ACTOR_HPP

#include <libdragon.h>
#include <t3d/t3d.h>

class ActorBase {
  public:
    ActorBase();
    float pos[3];
    float rot[3];
    float scale[3];
    void Update();
    void Draw();
  private:

    rspq_block_t *dpl;
    T3DMat4FP *modelMat;
};

#endif