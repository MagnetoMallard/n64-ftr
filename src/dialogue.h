//
// Created by mallard on 13/08/2025.
//

#ifndef DIALOGUE_H
#define DIALOGUE_H
#include <libdragon.h>

// CODE
typedef struct {
    char speakerName[32];
    char speakerText[141];
} TextBoxContent;

typedef struct {
    int boxScreenPosY;
    int boxScreenPosX;
    int boxWidth;
    int boxHeight;
    TextBoxContent* textBoxContent;
} TextBoxParams;


static void text_box(TextBoxParams* content) {
    static int border = 4;

    // text bg
    rdpq_set_mode_fill(RGBA32(128, 50, 128, 255));
    rdpq_fill_rectangle(
        content->boxScreenPosX,
        content->boxScreenPosY,
        content->boxScreenPosX + content->boxWidth,
        content->boxScreenPosY + content->boxHeight
    );
    // namebox bg
    rdpq_set_mode_fill(RGBA32(128+100, 50+100, 128+100, 255));
    rdpq_fill_rectangle(
        content->boxScreenPosX,
        content->boxScreenPosY -10,
        content->boxScreenPosX + (content->boxWidth / 2),
        content->boxScreenPosY
    );

    // Name Text
    rdpq_text_printf(&(rdpq_textparms_t){
                         .width = content->boxWidth,
                         .height = content->boxHeight,
                         .align = ALIGN_LEFT,
                         .valign = VALIGN_TOP,
                     },
                     4,
                     content->boxScreenPosX + border,
                     //this isn't right, but it works for now.
                     content->boxScreenPosY - 8,
                     content->textBoxContent->speakerName
    );

    // Content Text
    rdpq_text_printf(&(rdpq_textparms_t){
                         .width = content->boxWidth - border * 2,
                         .height = content->boxHeight - border * 2,
                         .align = ALIGN_LEFT,
                         .valign = VALIGN_TOP,
                         .wrap = WRAP_WORD,
                         .line_spacing = 1
                     },
                     4,
                     content->boxScreenPosX + border,
                     content->boxScreenPosY + border,
                     content->textBoxContent->speakerText
    );
};


// == DATA
TextBoxContent kob[3];
TextBoxContent dyn[4];
TextBoxContent rnb[4];

// Character Names
#define DYNAMO "Dynamo"
#define KOBOLD "Kobold"

// Textboxes
TextBoxContent kob[3] = {
    {KOBOLD, "i fukkin eat rocks and u cant stop me"},
    {KOBOLD, "wait is this u stoppin me?"},
    {KOBOLD, "well that fukin SUCKs..."}
};

TextBoxContent dyn[4] = {
    {DYNAMO, "[Doing things]"},
    {DYNAMO, "[Still doing things]"},
    {DYNAMO, "*stares at u*"},
    {DYNAMO, "Oh hey there lil guy! :) always nice to meet a fan! :)"}
};

TextBoxContent rnb[4] = {
    {"Redd & Blu", "*absolutely filled to the brim with girlish glee*"},
    {"Redd", "THIS IS THE BEST NIGHT OF LIFE."},
    {"Blu", "CONTINUE TO DANCE LEST YE INCUR OUR WRATH, MORTAL."},
    {"Redd & Blu", "WHEEEEEEEE! HOORAY! :)"}
};

#endif //DIALOGUE_H
