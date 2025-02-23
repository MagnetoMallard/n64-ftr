#include <string.h>
#include <libdragon.h>
#include <t3d/t3d.h>
#include <t3d/t3dmath.h>
#include <t3d/t3dmodel.h>
#include <graphics.h>

#include "actor.h"
#include "checkerboard.h"
#include "dragon.h"
#include "camera.h"
#include "lights.h"
#include "debug.h"

#define ACTOR_COUNT 2
#define DIRECTIONAL_LIGHT_COUNT 1

float get_time_s()  { return (float)((double)get_ticks_ms() / 1000.0); }
static inline void engine_init();
inline void t3d_draw_setup(T3DViewport* viewport, uint8_t* ambientLightColour,uint8_t* colorDir, T3DVec3* lightDirVec);
inline void debug_print(int x, int y, char* label, float value);

void music_init();
inline void music_update();

void print_inputs(_SI_condat *inputs);
bool mute[16] = {0};
xm64player_t xm;

int main()
{
  engine_init();

  music_init();

  static float objTime = 0.0f;

  T3DViewport viewport = t3d_viewport_create();

  Camera camera = camera_create();

  // ======== Init Lights
  uint8_t ambientLightColour[4] = {80, 80, 80, 0xFF};

  Light directionalLights[DIRECTIONAL_LIGHT_COUNT];

  uint8_t colorDir[4]     = {0xEE, 0xAA, 0xAA, 0xFF};
  T3DVec3 lightDirVec = {{-1.0f, 1.0f, 1.0f}};
  Light pointLightOne = light_create(colorDir, lightDirVec);

  directionalLights[0] = pointLightOne;

  // ======== Init Actors
  Actor actors[ACTOR_COUNT];

  Actor dragonActor = dragon_create(1);
  Actor checkerboardActor = checkerboard_create(2);

  actors[0] = dragonActor;
  actors[1] = checkerboardActor;

  for(;;)
  {
    // ======== Update
    controller_scan();
    struct controller_data inputs = get_keys_pressed();

    float newTime = get_time_s();
    float deltaTime = (newTime - objTimeLast) * baseSpeed;
    objTimeLast = newTime;
    objTime += deltaTime;
    
    for(int i = 0; i < ACTOR_COUNT; i++) {
      actor_update(&actors[i], objTime);
    }
    camera_update(&inputs.c[0], &camera, &viewport);

    // ======== Draw
    t3d_draw_setup(&viewport, ambientLightColour, colorDir, &lightDirVec);

    // ======= lights
    t3d_light_set_ambient(ambientLightColour);
    for(int i = 0; i < DIRECTIONAL_LIGHT_COUNT; i++) {
      light_draw(&directionalLights[i], i);
    }
    t3d_light_set_count(DIRECTIONAL_LIGHT_COUNT);

    // = <Inner Draw>
    actor_draw(&checkerboardActor);
    actor_draw(&dragonActor);
    
    // = </Inner Draw>

    // ====== 2D
    camera_draw(); // purely for debug
    // debug_print(16, 170, "DRAGON ROT Y", dragonActor.rot[1]);
    // debug_print(16, 220, "OBJTIME", objTime);
    rdpq_text_printf(NULL, FONT_BUILTIN_DEBUG_MONO, 16, 16, "STICK X : %.i",  inputs.c[0].x);
    rdpq_text_printf(NULL, FONT_BUILTIN_DEBUG_MONO, 16, 32, "CAMERA XTAR: %.2f",  camera.target.x);
    rdpq_text_printf(NULL, FONT_BUILTIN_DEBUG_MONO, 16, 48, "CAMERA ZTAR: %.2f",  camera.target.z);
    rdpq_text_printf(NULL, FONT_BUILTIN_DEBUG_MONO, 16, 64, "CAMERA XPOS: %.2f",  camera.pos.x);
    rdpq_text_printf(NULL, FONT_BUILTIN_DEBUG_MONO, 16, 80, "CAMERA ZPOS: %.2f",  camera.pos.z);
    rdpq_text_printf(NULL, FONT_BUILTIN_DEBUG_MONO, 16, 96, "FPS: %.2f",  display_get_fps());
    rdpq_detach_show();

    // ===== Audio
    music_update();

  }

  actor_delete(&dragonActor);
  actor_delete(&checkerboardActor);

  t3d_destroy();
  return 0;
}

static inline void engine_init() { 
  debug_init_isviewer();
  debug_init_usblog();
  asset_init_compression(2);

  dfs_init(DFS_DEFAULT_LOCATION);
  display_init(RESOLUTION_320x240, DEPTH_16_BPP, 3, GAMMA_NONE, FILTERS_RESAMPLE_ANTIALIAS);
  rdpq_init();
  timer_init();
  joypad_init();
  debug_init_isviewer();
  console_init();
  audio_init(44100, 4);
  mixer_init(16);

  t3d_init((T3DInitParams){});
  rdpq_text_register_font(FONT_BUILTIN_DEBUG_MONO, rdpq_font_load_builtin(FONT_BUILTIN_DEBUG_MONO));

}

inline void t3d_draw_setup(T3DViewport* viewport, uint8_t* ambientLightColour,uint8_t* colorDir, T3DVec3* lightDirVec) {
  rdpq_attach(display_get(), display_get_zbuf());
  t3d_frame_start();
  t3d_viewport_attach(viewport);
  rdpq_mode_combiner(RDPQ_COMBINER_SHADE);

  t3d_screen_clear_color(RGBA32(75, 0, 75, 0xFF));
  t3d_screen_clear_depth();
   t3d_state_set_drawflags(T3D_FLAG_SHADED | T3D_FLAG_DEPTH );
}

void music_init() {
  char *cur_rom = "rom:/smallhold.xm64";

  xm64player_open(&xm, cur_rom);
  xm64player_play(&xm, 0);

  	// Unmute all channels
	memset(mute, 0, sizeof(mute));
}

inline void music_update() {
  int audiosz = audio_get_buffer_length();
  extern int64_t __mixer_profile_rsp, __wav64_profile_dma;
  __mixer_profile_rsp = __wav64_profile_dma = 0;
  while (!audio_can_write()) {}

  int16_t *out = audio_write_begin();
  mixer_poll(out, audiosz);
  audio_write_end();
}