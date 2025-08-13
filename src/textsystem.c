//
// Created by CamashRed on 01/08/2025.
//

#include "globals.h"
#include "stage.h"
#include "main.c"
#include <libdragon.h>


//array of text

typedef struct{
    char speakerName[11];
    char speakerText[141];
}textSystems;

textSystems kob[3];
textSystems dyn[4];
textSystems rnb[4];

int textSegment = 3;
int i = 0;


int phh(){

switch(textSegment){
  case 1:
  do {
    printf("%s\n", kob[i].speakerName);
    printf("%s\n", kob[i].speakerText);
    i++;
    }
    while (i < 3);
    break;
  case 2:
  do {
    printf("%s\n", dyn[i].speakerName);
    printf("%s\n", dyn[i].speakerText);
    i++;
    }
    while (i < 4);
    break;
  case 3:
  do {
    printf("%s\n", rnb[i].speakerName);
    printf("%s\n", rnb[i].speakerText);
    i++;
    }
    while (i < 4);
    break;
  default:
    printf("Something is wrong, fuckface!");
    break;
    }
    return 0;
}

textSystems kob[3] = {
    {"Kobold", "i fukkin eat rocks and u cant stop me"},
    {"Kobold", "wait is this u stoppin me?"},
    {"Kobold", "well that fukin SUCKs..."}
};

textSystems dyn[4] = {
    {"Dynamo", "[Doing things]"},
    {"Dynamo", "[Still doing things]"},
    {"Dynamo", "*stares at u*"},
    {"Dynamo", "Oh hey there lil guy! :) always nice to meet a fan! :)"}
};

textSystems rnb[4] = {
    {"Redd & Blu", "*absolutely filled to the brim with girlish glee*"},
    {"Redd", "THIS IS THE BEST NIGHT OF LIFE."},
    {"Blu", "CONTINUE TO DANCE LEST YE INCUR OUR WRATH, MORTAL."},
    {"Redd & Blu", "WHEEEEEEEEEEEEE. :)"}
};


static void text_box(textBlock, int boxScreenposX, int boxScreenposY) {
    int border = 4;

    when press a, array ++, if > array end function

    rdpq_set_mode_fill(RGBA32(128,50,128,255));
    rdpq_fill_rectangle(
        boxScreenposX,
        boxScreenposY,
        boxScreenposX + boxWidth,
        boxScreenposY + boxHeight
    );

    rdpq_text_printf(&(rdpq_textparms_t) {
        .width = boxWidth - border*2,
        .height = boxHeight - border*2,
        .align = ALIGN_LEFT,
        .valign = VALIGN_CENTER,
        .wrap = WRAP_WORD,
        .line_spacing = 1
    }, 4, boxScreenposX + border, boxScreenposY + border, text);
};
