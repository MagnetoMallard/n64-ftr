//
// Created by mallard on 02/05/2025.
//

#include "hud.h"

#include <libdragon.h>

#include "stage.h"
#include "lights/light.h"
#include "../libs/libxm/xm.h"

static float spinTimer = 0.0f;
static float horizAnimationTimer = 0.0f;
static float vertAnimationTimer = 0.0f;
static Light directionalLights[DIRECTIONAL_LIGHT_COUNT];

static void sine_text(const char* text, float speedFactor, float xOffset, float yOffset, bool scroll ) {
    int strLen = strlen(text);

    int xScroll = scroll ? horizAnimationTimer * speedFactor : 0;

    for (int i = 0; i < strLen; i++) {
        rdpq_font_style(ftrFont, 1, &(rdpq_fontstyle_t){
          .color = hsla2rgba( 0.01f * spinTimer,fm_sinf(spinTimer + i),0.5f,1.0f),
          .outline_color = RGBA32(0,0,0,255),
        });
        rdpq_text_printn(
        &(rdpq_textparms_t) {
            .style_id = 1
        },
        3,
        fm_fmodf(xScroll  + (i * 12), display_get_width()) + xOffset,
        (fm_sinf(horizAnimationTimer + i) * speedFactor) + yOffset,
        &text[i], 1);
    }

}

void hud_setup() {
    stage_get_directional_lights();
}

void hud_update() {

}

void hud_draw() {
    int musicTitlePos = display_get_height() - charHeight*4;
    sine_text(xm_get_module_name(xm.ctx), 4.0f, 32.0f ,  musicTitlePos, false);
    rdpq_text_printf(nullptr, 3, 220.0f ,  fpsPos, lightBehaviourArray[lightBehaviourIndex].name);
}

void hud_teardown() {

}

void hud_debug_draw() {
    rdpq_text_printf(nullptr, 3, margin, fpsPos, "FPS: %.2f", display_get_fps());
}
