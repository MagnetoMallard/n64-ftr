#include <string.h>
#include <libdragon.h>
#include <t3d/t3d.h>

#include "gloabls.h"
#include "stage.h"

float get_time_s()  { return (float)((double)get_ticks_ms() / 1000.0); }

static void engine_init();
static void engine_teardown();

static void music_init();

bool mute[SOUND_CHANNELS] = {0};
xm64player_t xm;

enum GameSate gameState = STAGE;
struct controller_data inputs;

int main()
{
  engine_init();

  music_init();

  int isSetup = 0;
  int isRunning = 1;

  while (isRunning) {
    controller_scan();
    inputs = get_keys_pressed();

    switch (gameState) {
      default:
      case STAGE:
        if (isSetup == 0) { isSetup = stage_setup(); }
        stage_loop(1);
        break;
      case PAUSED:
        stage_loop(0);
        if(inputs.c[0].B) { isRunning = 0; }
        break;
    }
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
  display_init(RESOLUTION_320x240, DEPTH_16_BPP, 3, GAMMA_NONE, FILTERS_RESAMPLE_ANTIALIAS_DEDITHER);
  rdpq_init();
  timer_init();
  joypad_init();
  debug_init_isviewer();
  // console_init();
  audio_init(44100, 8);
  mixer_init(SOUND_CHANNELS);
  t3d_init((T3DInitParams){});
  rdpq_text_register_font(FONT_BUILTIN_DEBUG_MONO, rdpq_font_load_builtin(FONT_BUILTIN_DEBUG_MONO));
}

static void engine_teardown() {

  t3d_destroy();
}


void music_init() {
  char *cur_rom = "rom:/smallhold.xm64";
  xm64player_open(&xm, cur_rom);
  xm64player_play(&xm, 0);

  // Unmute all channels
  memset(mute, 0, sizeof(mute));
}


