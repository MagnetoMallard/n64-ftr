//
// Created by mallard on 19/03/2025.
//

#ifndef STAGE_H
#define STAGE_H

int stage_setup();
void stage_take_input(enum GameSate passedGameState);
void stage_render_frame(enum GameSate passedGameState);
void stage_teardown();

#endif //STAGE_H
