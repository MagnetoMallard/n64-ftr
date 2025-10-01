#include <libdragon.h>
#include <usb.h>
#include <t3d/t3d.h>
#include "globals.h"
#include "stage.h"
#include "audio/audio_playback.h"

enum GameState gameState = STAGE;

float get_time_s() { return (float) ((double) get_ticks_ms() / 1000.0); }

static void engine_init();
static void engine_teardown();
static void inputs_update();
static void load_fonts();



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

    switch (gameState) {
      default:
      case STAGE:
        if (isSetup == 0) { isSetup = stage_setup(); }
        audio_playback_try_play();
        stage_take_input(STAGE);
        stage_render_frame(STAGE);
        audio_playback_try_play(); // make a couple attempts, as is recommended
        break;
      case PAUSED:
        stage_take_input(PAUSED);
        stage_render_frame(PAUSED);
        break;
    }

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
    .pal60 = true,
    .overscan_margin = VI_CRT_MARGIN
  };
  const resolution_t RESOLUTION_640x480_PAL = {
    .width = 640,
    .height = 480,
    .interlaced = INTERLACE_HALF,
    .pal60 = true
  };
    const resolution_t RESOLUTION_360x288_PAL = {
    .width = 360,
    .height = 288,
    .interlaced = INTERLACE_HALF,
    .pal60 = true,
    //.overscan_margin = VI_CRT_MARGIN
  };

  display_init(RESOLUTION_320x240, DEPTH_16_BPP, 4, GAMMA_NONE, FILTERS_RESAMPLE);
  display_set_fps_limit(30);
  rdpq_init();
  timer_init();
  joypad_init();
  timer_init(); // needed for hashmaps!
  yuv_init();
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
//Base fontstyles (0) are all white with black outlines.
static void load_fonts()
	{

  //Main FTR font
  ftrFont = rdpq_font_load("rom:/STAN0753.font64");
  rdpq_text_register_font(3, ftrFont);

  	rdpq_font_style(ftrFont, 0, &(rdpq_fontstyle_t){
      .color = RGBA32(225, 255, 255, 255),
      .outline_color = RGBA32(0, 0, 0, 255),
    });

  	rdpq_font_style(ftrFont, 1, &(rdpq_fontstyle_t){
      .color = RGBA32(100, 255, 255, 255),
      .outline_color = RGBA32(0, 0, 100, 255),
    });

  //Main FTR font, but skinny
  ftrFontSkinny = rdpq_font_load("rom:/STAN0754.font64");
  rdpq_text_register_font(4, ftrFontSkinny);

    rdpq_font_style(ftrFontSkinny, 0, &(rdpq_fontstyle_t){
      .color = RGBA32(225, 255, 255, 255),
      .outline_color = RGBA32(0, 0, 0, 255),
    });

    rdpq_font_style(ftrFontSkinny, 1, &(rdpq_fontstyle_t){
      .color = RGBA32(225, 255, 255, 255),
      .outline_color = RGBA32(0, 0, 0, 255),
    });

  //V5PRC___
  V5PRC___ = rdpq_font_load("rom:/V5PRC___.font64");
  rdpq_text_register_font(5, V5PRC___);

   	rdpq_font_style(V5PRC___, 0, &(rdpq_fontstyle_t){
      .color = RGBA32(255, 255, 0, 255),
    });

  //Main FTR font but BIG.
  ftrFontBig = rdpq_font_load("rom:/STAN0753big.font64");
  rdpq_text_register_font(6, ftrFontBig);

  	rdpq_font_style(ftrFontBig, 0, &(rdpq_fontstyle_t){
      .color = RGBA32(225, 255, 255, 255),
      .outline_color = RGBA32(0, 0, 0, 255),
    });

  	rdpq_font_style(ftrFontBig, 1, &(rdpq_fontstyle_t){
      .color = RGBA32(100, 255, 255, 255),
      .outline_color = RGBA32(0, 0, 100, 255),
    });

}
