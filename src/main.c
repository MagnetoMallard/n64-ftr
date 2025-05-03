#include <libdragon.h>
#include <usb.h>
#include <t3d/t3d.h>
#include "globals.h"
#include "stage.h"
#include "audio/audio_playback.h"

float get_time_s() { return (float) ((double) get_ticks_ms() / 1000.0); }

static void engine_init();
static void engine_teardown();
static void inputs_update();
static void load_fonts();

enum GameSate gameState = STAGE;

joypad_inputs_t inputs;
joypad_buttons_t btnsUp;
joypad_buttons_t btnsPressed;

int main() {
  engine_init();
  audio_playback_init();
  load_fonts();

  int isSetup = 0;
  int isRunning = 1;

  usb_initialize();

  while (isRunning) {
    inputs_update();
    audio_playback_take_input();

    audio_playback_try_play();

    switch (gameState) {
      default:
      case STAGE:
        if (isSetup == 0) { isSetup = stage_setup(); }
      //  audio_set_master_fx(NONE);
        audio_playback_try_play();
        stage_take_input(STAGE);
        stage_render_frame(STAGE);
        break;
      case PAUSED:
        audio_set_master_fx(PHASE);
        stage_take_input(PAUSED);
        stage_render_frame(PAUSED);
        break;
    }

    // make a couple attempts, as is recommended

    if (usb_poll()) {
      isRunning = 0;
    };
  }

  stage_teardown();

  engine_teardown();

  return 0;
}

static void engine_init() {
  debug_init_isviewer();
  debug_init_usblog();
  asset_init_compression(2);

  dfs_init(DFS_DEFAULT_LOCATION);
  const resolution_t RESOLUTION_320x240_PAL = {
    .width = 320,
    .height = 240,
    .interlaced = INTERLACE_OFF,
    .pal60 = true
  };
  const resolution_t RESOLUTION_640x480_PAL = {
    .width = 640,
    .height = 480,
    .interlaced = INTERLACE_HALF,
    .pal60 = true
  };

  display_init(RESOLUTION_320x240_PAL, DEPTH_16_BPP, 3, GAMMA_NONE, FILTERS_RESAMPLE);
  rdpq_init();
  timer_init();
  joypad_init();
  timer_init(); // needed for hashmaps!
  debug_init_isviewer();
  t3d_init((T3DInitParams){});
  rdpq_text_register_font(FONT_BUILTIN_DEBUG_MONO, rdpq_font_load_builtin(FONT_BUILTIN_DEBUG_MONO));
}

static void engine_teardown() {
  t3d_destroy();
}

static void inputs_update() {
  joypad_poll();
  btnsUp = joypad_get_buttons_released(0);
  btnsPressed = joypad_get_buttons_pressed(0);
  inputs = joypad_get_inputs(0);
}


//Let's give some font loading a try here. Font ID 0 and 1 are already registered.
static void load_fonts() {
  ftrFont = rdpq_font_load("rom:/STAN0753.font64");
  rdpq_text_register_font(3, ftrFont);

  rdpq_font_t* fnt4 = rdpq_font_load("rom:/BitDotted.font64");
  rdpq_text_register_font(4, fnt4);

  //use the fonts with different colours, outlines, etc
  rdpq_font_style(fnt4,
                  0,
                  &(rdpq_fontstyle_t){
                    .color = RGBA32(255, 255, 0, 255),
                    .outline_color = RGBA32(0, 0, 0, 255),
                  }
  );

  rdpq_font_style(ftrFont,
                  1,
                  &(rdpq_fontstyle_t){
                    .color = RGBA32(255, 255, 255, 255),
                    .outline_color = RGBA32(0, 0, 0, 255),
                  }
  );
}
