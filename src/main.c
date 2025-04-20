#include <string.h>
#include <libdragon.h>
#include <usb.h>
#include <t3d/t3d.h>
#include <limits.h>
#include "globals.h"
#include "stage.h"
#include "actors/actor.h"
#include "audio/audio_fx.h"


float get_time_s()  { return (float)((double)get_ticks_ms() / 1000.0); }

static void engine_init();
static void engine_teardown();
static void music_load(int songIndex);
static void music_init();
static void mixer_try_play_custom();

bool mute[SOUND_CHANNELS] = {0};
xm64player_t xm;

enum GameSate gameState = STAGE;
enum GameSate musicState = HOLDLOOP;
enum MasterFxType audioMixerMasterFx = NONE;

joypad_inputs_t inputs;
joypad_buttons_t btnsUp;
joypad_buttons_t btnsPressed;

char* songs[SONG_COUNT];

uint8_t songSelection = 0;

//Let's give some font loading a try here. Font ID 0 and 1 are already registered.
static void load_fonts() {
  ftrFont = rdpq_font_load("rom:/STAN0753.font64");
  rdpq_text_register_font(3, ftrFont);

  rdpq_font_t *fnt4 = rdpq_font_load("rom:/BitDotted.font64");
  rdpq_text_register_font(4, fnt4);

  //use the fonts with different colours, outlines, etc
  rdpq_font_style(fnt4,0,&(rdpq_fontstyle_t){
  .color = RGBA32(255,255,0,255),
  .outline_color = RGBA32(0,0,0,255),
  });

  rdpq_font_style(ftrFont,1,&(rdpq_fontstyle_t){
  .color = RGBA32(255,255,255,255),
  .outline_color = RGBA32(0,0,0,255),
  });

}

int main()
{
  engine_init();
  load_fonts();
  music_init();

  int isSetup = 0;
  int isRunning = 1;

  usb_initialize();

  while (isRunning) {

    joypad_poll();
    btnsUp = joypad_get_buttons_released(0);
    btnsPressed = joypad_get_buttons_pressed(0);
    inputs = joypad_get_inputs(0);

    // trigger on button release
    if (btnsPressed.l) {
      songSelection--;
      music_load(songSelection % SONG_COUNT );
    }

    // trigger on button release
    if (btnsPressed.r) {
      songSelection++;
      music_load(songSelection % SONG_COUNT );
    }

    mixer_try_play_custom();

    switch (gameState) {
      default:
      case STAGE:
        if (isSetup == 0) { isSetup = stage_setup(); }
        stage_loop(1);
        break;
      case PAUSED:
        stage_loop(0);
        // if(inputs.btn.b) { isRunning = 0; }
        break;
    }

    mixer_try_play_custom(); // make a couple attempts, as is recommended

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
  const resolution_t RESOLUTION_320x240_PAL = {.width = 320, .height = 240, .interlaced = INTERLACE_OFF, .pal60 = true};
  const resolution_t RESOLUTION_640x480_PAL = {.width = 640, .height = 480, .interlaced = INTERLACE_HALF, .pal60 = true};

  display_init(RESOLUTION_320x240_PAL, DEPTH_16_BPP, 3, GAMMA_NONE, FILTERS_RESAMPLE);

  rdpq_init();
  timer_init();
  joypad_init();
  timer_init(); // needed for hashmaps!
  debug_init_isviewer();
  audio_init(44100, 8);
  mixer_init(SOUND_CHANNELS);
  t3d_init((T3DInitParams){});
  rdpq_text_register_font(FONT_BUILTIN_DEBUG_MONO, rdpq_font_load_builtin(FONT_BUILTIN_DEBUG_MONO));
}

static void engine_teardown() {
  t3d_destroy();
}


static void music_load(int songIndex) {
  if (xm.playing) {
    xm64player_close(&xm);
  }
  xm64player_open(&xm, songs[songIndex]);
  xm64player_play(&xm, 0);

}

static void music_init() {
  debugf("blah");
  // SETUP SONG LIST
  songs[0] = "rom:/smallhold.xm64";
  songs[1] = "rom:/kritta-girl.xm64";
  songs[2] = "rom:/field-day.xm64";

  music_load(0);

  // Unmute all channels
  memset(mute, 0, sizeof(mute));
}

static void mixer_try_play_custom() {

  if (audio_can_write())
  {
    int bufferLength = audio_get_buffer_length() ;
    short *buf = audio_write_begin();
    mixer_poll(buf, bufferLength);

    static float filterTimer = 0;
    float sinSweep = (fm_cosf(filterTimer+=0.05f) + 1.0f) * 0.4f;

    switch (audioMixerMasterFx) {
      case RESONANT_LP_SWEEP:
        audio_fx_lopass_resonant(buf, bufferLength<<1, sinSweep, 0.2f);
      break;
      case FIXEDPONT_LP_SWEEP:
        audio_fx_lopass_fp(buf, bufferLength<<1, sinSweep);
      break;
      case RESONANT_LP_DIVE:
        audio_fx_lopass_resonant(buf, bufferLength<<1, 0.11f, 0.5f);
      break;
      case FIXEDPONT_LP_DIVE:
        audio_fx_lopass_fp(buf, bufferLength<<1, 0.2f);
      break;
      case NONE:
        default:
      }

    audio_write_end();
  }
}

