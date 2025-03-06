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

#define ACTOR_COUNT 3
#define DIRECTIONAL_LIGHT_COUNT 3

float get_time_s()  { return (float)((double)get_ticks_ms() / 1000.0); }
static inline void engine_init();
static inline void t3d_draw_setup(T3DViewport* viewport, uint8_t* ambientLightColour,uint8_t* colorDir, T3DVec3* lightDirVec);
inline void debug_print(int x, int y, char* label, float value);

void music_init();
inline void music_update();

float fogNear = 100.0;
float fogFar = 250.0;

void print_inputs(_SI_condat *inputs);
bool mute[16] = {0};
xm64player_t xm;

int main()
{
  engine_init();

  music_init();

  static float objTime = 0.0f;
  static int audioTimer = 0;

  T3DViewport viewport = t3d_viewport_create();

  Camera camera = camera_create();

  // ======== Init Lights
  uint8_t ambientLightColour[4] = {80, 80, 80, 0xFF};

  Light directionalLights[DIRECTIONAL_LIGHT_COUNT];

  uint8_t colorDir[4]     = {0xEE, 0xAA, 0xAA, 0xFF};
  T3DVec3 lightDirVec = {{-1.0f, 1.0f, 1.0f}};
  Light pointLightOne = light_create(colorDir, lightDirVec);

  uint8_t colorDir2[4]     = {0xFF, 0x00, 0x00, 0xFF};
  T3DVec3 lightDirVec2 = {{-1.0f, 1.0f, -1.0f}};
  Light pointLightTwo = light_create(colorDir2, lightDirVec2);
  
  uint8_t colorDir3[4]     = {0x00, 0x00, 0xFF, 0xFF};
  T3DVec3 lightDirVec3 = {{1.0f, 1.0f, 1.0f}};
  Light pointLightThree = light_create(colorDir3, lightDirVec3);

  directionalLights[0] = pointLightOne;
  directionalLights[1] = pointLightTwo;
  directionalLights[2] = pointLightThree;

  // ======== Init Actors
  Actor actors[ACTOR_COUNT];

  Actor dragonActor = dragon_create(1);
  Actor checkerboardActor = checkerboard_create(2);
  Actor stageActor = create_actor_from_model(3, "MainBarArea");

  actors[0] = dragonActor;
  actors[1] = checkerboardActor;
  actors[2] = stageActor;

  for(;;)
  {
    // ======== Update
    controller_scan();
    struct controller_data inputs = get_keys_pressed();

    float newTime = get_time_s();
    float deltaTime = (newTime - objTimeLast) * baseSpeed;
    objTimeLast = newTime;
    objTime += deltaTime;

    // for(int i = 0; i < ACTOR_COUNT; i++) {
    //   actor_update(&actors[i], objTime);
    // }
    camera_update(&inputs.c[0], &camera, &viewport);
    
    if(inputs.c[0].up) fogNear--;
    if(inputs.c[0].down) fogNear++;

    if(inputs.c[0].left) fogFar--;
    if(inputs.c[0].right) fogFar++;
    
    // ======== Draw
    t3d_draw_setup(&viewport, ambientLightColour, colorDir, &lightDirVec);

    // ======= lights
    t3d_light_set_ambient(ambientLightColour);
    for(int i = 0; i < DIRECTIONAL_LIGHT_COUNT; i++) {
      light_draw(&directionalLights[i], i);
    }
    t3d_light_set_count(DIRECTIONAL_LIGHT_COUNT);

    // = <Inner Draw>
    actor_draw(&stageActor);
    // actor_draw(&checkerboardActor);
    actor_draw(&dragonActor);

    // = </Inner Draw>

    // ====== 2D
    camera_draw(); // purely for debug
    // debug_print(16, 170, "DRAGON ROT Y", dragonActor.rot[1]);
    // debug_print(16, 220, "OBJTIME", objTime);
    rdpq_text_printf(NULL, FONT_BUILTIN_DEBUG_MONO, 16, 16, "STICK X : %.i",  inputs.c[0].x);
    rdpq_text_printf(NULL, FONT_BUILTIN_DEBUG_MONO, 16, 32, "FOG NEAR: %.2f",  fogNear);
    rdpq_text_printf(NULL, FONT_BUILTIN_DEBUG_MONO, 16, 48, "FOG FAR: %.2f",  fogFar);
    rdpq_text_printf(NULL, FONT_BUILTIN_DEBUG_MONO, 16, 64, "CAMERA XPOS: %.2f",  camera.pos.x);
    rdpq_text_printf(NULL, FONT_BUILTIN_DEBUG_MONO, 16, 80, "CAMERA ZPOS: %.2f",  camera.pos.z);
    rdpq_text_printf(NULL, FONT_BUILTIN_DEBUG_MONO, 16, 96, "FPS: %.2f",  display_get_fps());
    rdpq_detach_show();

    // ===== Audio
    mixer_try_play();


  }

  actor_delete(&dragonActor);
  actor_delete(&stageActor);
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
  audio_init(44100, 8);
  mixer_init(8);
  // t3d_fog_set_enabled(true);
  t3d_init((T3DInitParams){});
  rdpq_text_register_font(FONT_BUILTIN_DEBUG_MONO, rdpq_font_load_builtin(FONT_BUILTIN_DEBUG_MONO));

}

static inline void t3d_draw_setup(T3DViewport* viewport, uint8_t* ambientLightColour,uint8_t* colorDir, T3DVec3* lightDirVec) {
  rdpq_attach(display_get(), display_get_zbuf());
  t3d_frame_start();
  t3d_viewport_attach(viewport);
  rdpq_mode_combiner(RDPQ_COMBINER_SHADE);

  rdpq_mode_fog(RDPQ_FOG_STANDARD);
  rdpq_set_fog_color((color_t){70, 70, 140, 0xFF});

  t3d_screen_clear_color(RGBA32(70, 70, 140, 0xFF));
  t3d_screen_clear_depth();

  t3d_fog_set_range(fogNear, fogFar);
  t3d_fog_set_enabled(true);
  t3d_state_set_drawflags(T3D_FLAG_SHADED | T3D_FLAG_DEPTH );
}

void music_init() {
  char *cur_rom = "rom:/smallhold.xm64";

  xm64player_open(&xm, cur_rom);
  xm64player_play(&xm, 0);

  // Unmute all channels
  memset(mute, 0, sizeof(mute));

  // set an update function
  // audio_set_buffer_callback(&music_update_callback);
}

inline void music_update() {
  int audiosz = audio_get_buffer_length();
  int16_t *out = audio_write_begin();
  mixer_poll(out, audiosz);
  audio_write_end();
}

