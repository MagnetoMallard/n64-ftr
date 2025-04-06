
#include <libdragon.h>
#include <t3d/t3d.h>

#include "Actor.hpp"


ActorBase::ActorBase() { 
  // h
} 

void ActorBase::Update() { 
  // h

}

void ActorBase::Draw() { 
  t3d_matrix_push_pos(1);
  t3d_matrix_set(modelMat, true);
  rspq_block_run(dpl);
  t3d_matrix_pop(1);
}