#include <string.h>
#include <libdragon.h>
#include <t3d/t3d.h>
#include <t3d/t3dmath.h>
#include <t3d/t3dmodel.h>
#include <graphics.h>

#include "actor.h"
#include "checkerboard.h"
#include "dragon.h"

color_t get_rainbow_color(float s) {
  float r = fm_sinf(s + 0.0f) * 127.0f + 128.0f;
  float g = fm_sinf(s + 2.0f) * 127.0f + 128.0f;
  float b = fm_sinf(s + 4.0f) * 127.0f + 128.0f;
  return RGBA32(r, g, b, 255);
}

void print_inputs(_SI_condat *inputs)
{
    printf(
        "Stick: %+04d,%+04d\n",
        inputs->x, inputs->y
    );
    printf(
        "D-U:%d D-D:%d D-L:%d D-R:%d C-U:%d C-D:%d C-L:%d C-R:%d\n",
        inputs->up, inputs->down,
        inputs->left, inputs->right,
        inputs->C_up, inputs->C_down,
        inputs->C_left, inputs->C_right
    );
    printf(
        "A:%d B:%d L:%d R:%d Z:%d Start:%d\n",
        inputs->A, inputs->B,
        inputs->L, inputs->R,
        inputs->Z, inputs->start
    );
}

float get_time_s()  { return (float)((double)get_ticks_ms() / 1000.0); }

/**
 * Simple example with a 3d-model file created in blender.
 * This uses the builtin model format for loading and drawing a model.
 */
int main()
{
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

  static float objTime = 0.0f;

  t3d_init((T3DInitParams){});
  T3DViewport viewport = t3d_viewport_create();
  rdpq_text_register_font(FONT_BUILTIN_DEBUG_MONO, rdpq_font_load_builtin(FONT_BUILTIN_DEBUG_MONO));

  T3DMat4 modelMat = { 
    .m = {
      {1.0,0.0,0.0,0.0},
      {0.0,1.0,0.0,0.0},
      {0.0,0.0,1.0,0.0},
      {0.0,0.0,0.0,1.0}
    }
  };
 // t3d_mat4_identity(&modelMat);

  // Now allocate a fixed-point matrix, this is what t3d uses internally.
  // Note: this gets DMA'd to the RSP, so it needs to be uncached.
  // If you can't allocate uncached memory, remember to flush the cache after writing to it instead.
  T3DMat4FP* modelMatFP = malloc_uncached(sizeof(T3DMat4FP));

  const T3DVec3 camPos = {{0.0f,50.0f,175.0f}};
  T3DVec3 camTarget = {{0,50,0}};

  uint8_t colorAmbient[4] = {80, 80, 80, 0xFF};
  uint8_t colorDir[4]     = {0xEE, 0xAA, 0xAA, 0xFF};
  //colorDir values are ascii. 0xEE = 238. Value2 changes the colour, Value3 brightness/diffuse?

  T3DVec3 lightDirVec = {{-1.0f, 1.0f, 1.0f}};
  t3d_vec3_norm(&lightDirVec);


  // Create an actor for the derg
  Actor dragonActor = dragon_create(1);
  Actor checkerboardActor = checkerboard_create(2);

  float rotAngle = 0.0f;
  float rotAngleY = 0.0f;
  rspq_block_t *dplDraw = NULL;

  for(;;)
  {
    // ======== Update ======== //

    controller_scan();
    struct controller_data inputs = get_keys_pressed();

    int port = 0; 

  //  print_inputs(&inputs.c[port]);

    rotAngle -= ((&inputs.c[0])->x);
    rotAngleY -= ((&inputs.c[0])->y);

    float modelScale = 1.0;
    camTarget.x = -(rotAngle*0.1);
    camTarget.z = (cos(rotAngle*0.1));

    float newTime = get_time_s();
    float deltaTime = (newTime - objTimeLast) * baseSpeed;
    objTimeLast = newTime;
    objTime += deltaTime;

    actor_update(&dragonActor, objTime);
    actor_update(&checkerboardActor, objTime);
    
    t3d_viewport_set_projection(&viewport, T3D_DEG_TO_RAD(80.0f), 1.0f, 100.0f);
    t3d_viewport_look_at(&viewport, &camPos, &camTarget, &(T3DVec3){{0,1,0}});

    // ======== Draw ======== //
    rdpq_attach(display_get(), display_get_zbuf());
    t3d_frame_start();
    t3d_viewport_attach(&viewport);
    rdpq_mode_combiner(RDPQ_COMBINER_SHADE);

    t3d_screen_clear_color(RGBA32(75, 0, 75, 0xFF));
    t3d_screen_clear_depth();

    t3d_light_set_ambient(colorAmbient);
    t3d_light_set_directional(0, colorDir, &lightDirVec);
    t3d_light_set_count(1);
    t3d_state_set_drawflags(T3D_FLAG_SHADED | T3D_FLAG_DEPTH);

    t3d_matrix_push_pos(1);
    // ======== <Inner Draw> ======== //

    actor_draw(&checkerboardActor);
    actor_draw(&dragonActor);
    
    // ======== </Inner Draw> ======== //  
    t3d_matrix_pop(1);

    // ==== 2D ====== //
    rdpq_text_printf(NULL, FONT_BUILTIN_DEBUG_MONO, 16, 220, "OBJTIME : %.2f", objTime);
    rdpq_text_printf(NULL, FONT_BUILTIN_DEBUG_MONO, 16, 150, "DRAGON ROT X : %.2f", dragonActor.rot[0]);
    rdpq_text_printf(NULL, FONT_BUILTIN_DEBUG_MONO, 16, 170, "DRAGON ROT Y : %.2f", dragonActor.rot[1]);
    rdpq_text_printf(NULL, FONT_BUILTIN_DEBUG_MONO, 16, 190, "DRAGON ROT Z : %.2f", dragonActor.rot[2]);


    rdpq_detach_show();
  }

  actor_delete(&dragonActor);
  actor_delete(&checkerboardActor);

  t3d_destroy();
  return 0;
}
