
#include "../libs/libxm/xm.h"
#include "../libs/libxm/xm_internal.h"
#include <libdragon.h>

// Helpers
#include "helpers/debug_draw.h"
#include "helpers/aabb_helpers.h"
#include "helpers/colour_helpers.h"

#include "lights/light.h"
#include "lights/light_behaviours.h"

#include "actors/actor.h"
#include "actors/actor_behaviours.h"

#include "audio/audio_playback.h"

#include "camera/camera.h"

#include "globals.h"
#include "stage.h"

#define ACTORS_COUNT 13
#define DIRECTIONAL_LIGHT_COUNT 2

static Actor actors[ACTORS_COUNT];
static Camera camera;
static bool _cameraDirty;

static float deltaTime = 0.0f;
static float fogNear = 150.0f;
static float fogFar = 300.0f;
//static color_t fogColour = {70, 70, 70, 0xFF};
static color_t fogColour = {87, 110, 168, 0xFF}; //nice colour :)
//static color_t fogColour = {255, 255, 255, 0xFF};

static float spinTimer = 0.0f;
static float horizAnimationTimer = 0.0f;
static float vertAnimationTimer = 0.0f;

static T3DViewport viewport;
static Light directionalLights[DIRECTIONAL_LIGHT_COUNT];
uint8_t ambientLightColour[4] = {100, 100, 100, 0x7f};
rspq_syncpoint_t syncPoint = 0;
rdpq_font_t* ftrFont;
rdpq_font_t* V5PRC___;
rdpq_font_t* ftrFontSkinny;
rdpq_font_t* ftrFontBig;
surface_t* disp;

int randintx = 0;
int randinty = 0;
int* randx = &randintx;
int* randy = &randinty;

static LightBehaviour lightBehaviourArray[3] = {
{
    .name = "Synced Traffic Light",
    .updateFunction = &light_update_traffic_light_xm
    },
{
    .name = "Synced Tekno Strobe",
    .updateFunction = &light_update_xm_tekno_strobe
    },
{
    .name = "Mono Volume Follow",
    .updateFunction = &light_update_vol_follow
    },
};

static int lightBehaviourIndex = 0;

static void t3d_draw_update(T3DViewport *viewport);

static void debug_prints();
static void regular_prints();
static void draw_aabbs(Actor* curActor);

void text_box(const char text, int box_screenpos_x, int box_screenpos_y);

static void check_aabbs(Actor *curActor);
//static void sine_text(const char* text, float speedFactor, float xOffset, float yOffset, bool scroll );

static sprite_t* playBtnUpSprite;
static sprite_t* playBtnDownSprite;
static sprite_t* trackBackSprite;
static sprite_t* trackFwdSprite;
static sprite_t* dragonBackdrop;
static sprite_t* transBG1;
static rspq_block_t* hudBlock;

// ==== PUBLIC ====
int stage_setup() {
    viewport = t3d_viewport_create();

    camera = camera_create();
    //disp = display_get();
    // ======== Init Lights
    uint8_t colorDir[4] = {0x00, 0x00, 0x00, 0xFF};
    T3DVec3 lightDirVec = {{0.0f, 1.0f, 0.0f}};
    Light pointLightOne = light_create(colorDir, lightDirVec, false);
    pointLightOne.lightUpdateFunction = lightBehaviourArray[0].updateFunction;

    uint8_t colorDir2[4] = {0xFF, 0x44, 0x44, 0xFF};
    T3DVec3 lightDirVec2 = {{0.0f, 0.0f, 1.0f }};
    Light pointLightTwo = light_create(colorDir2, lightDirVec2, false);

    directionalLights[0] = pointLightOne;
    directionalLights[1] = pointLightTwo;

    // ======== Init Actors
    Actor dragonActor = create_actor_from_model("dragon2");
    Actor stageActor = create_actor_from_model("MainBarArea");
    Actor dynamoActor = create_actor_from_model("DynamoAnimation");

    //Init Stage Kobolds
    Actor koboldActor = create_actor_from_model("KoboldWithAnims");
    Actor koboldActor2 = create_actor_from_model("KoboldWithAnims");

    //Init Usher Kobolds
    Actor usherKoboldActor = create_actor_from_model("KoboldCelUsher");
    Actor usherKoboldActor2 = create_actor_from_model("KoboldCelUsher");

    //Init Dancefloor Kobolds
    Actor koboldActor3 = create_actor_from_model("KoboldWithAnims");
    Actor koboldActor4 = create_actor_from_model("KoboldWithAnims");
    Actor koboldActor5 = create_actor_from_model("KoboldWithAnims");
    Actor koboldActor6 = create_actor_from_model("KoboldWithAnims");
    Actor koboldActor7 = create_actor_from_model("KoboldWithAnims");
    Actor koboldActor8 = create_actor_from_model("KoboldWithAnims");

    //I don't know why this is here but honestly I'm scared to move it at this point.
    dynamo_init();

    //---
    //Colour change of the kobolds, RGBA. Final alpha seems to be unnecessary.
    //Set kobold model's vertex colours to white.
    //---

    //Colour Stage Kobolds
    kobold_init(&koboldActor, (color_t){255,255,255}, (color_t){255,0,0});
    kobold_init(&koboldActor2, (color_t){255,100,100}, (color_t){24,0,100});

    //Colour Usher Kobolds
    kobold_init(&usherKoboldActor, (color_t){255,200,255}, (color_t){255,0,100});
    kobold_init(&usherKoboldActor2, (color_t){255,255,200}, (color_t){24,0,100});

    //Colour Dancefloor kobolds
    kobold_init(&koboldActor3, (color_t){0,100,100}, (color_t){0,100,100});
    kobold_init(&koboldActor4, (color_t){100,100,255}, (color_t){20,100,15});
    kobold_init(&koboldActor5, (color_t){100,255,100}, (color_t){100,80,100});
    kobold_init(&koboldActor6, (color_t){255,100,100}, (color_t){255,80,80});
    kobold_init(&koboldActor7, (color_t){100,0,100}, (color_t){0,0,100});
    kobold_init(&koboldActor8, (color_t){100,100,0}, (color_t){255,100,0});

    dragonActor.updateFunction = &dragon_update;
    koboldActor2.updateFunction = &dragon_update;
    dynamoActor.customPartDrawFunc = &dynamo_part_draw;

	//START POSITIONS, ROTATION, AND ANIM
    //Start postions of actors as a vec3, 0 on all vecs are centred. .pos[1] is equal to Blender's Z direction (Y here?)
    //Models start faced towards the entrance, there's be a better way to represent this later I'm sure.

    //DJ
    dynamoActor.pos[0] = -340.0f;
    dynamoActor.pos[1] = 50.0f;
    dynamoActor.pos[2] = -100.0f;
    dynamoActor.rot[1] = T3D_DEG_TO_RAD(-90);

    //Stage kobolds
    koboldActor.pos[0] = -284.0f;
    koboldActor.pos[1] = 10.0f;
    koboldActor.pos[2] = 100.0f;
    koboldActor.rot[1] = T3D_DEG_TO_RAD(-90);
    animations_change(&koboldActor.anim, 2, 0.3f);

    koboldActor2.pos[0] = -260.0f;
    koboldActor2.pos[1] = 10.0f;
    koboldActor2.pos[2] = -230.0f;
    koboldActor2.rot[1] = T3D_DEG_TO_RAD(-90);
    animations_change(&koboldActor2.anim, 1, 0.3f);

    //Usher Kobolds
    usherKoboldActor.pos[0] = 75.0f;
    usherKoboldActor.pos[1] = 20.0f;
    usherKoboldActor.pos[2] = 610.0f;
    usherKoboldActor.rot[1] = T3D_DEG_TO_RAD(-100);
    animations_change(&usherKoboldActor.anim, 3, 0.2f);

    usherKoboldActor2.pos[0] = 75.0f;
    usherKoboldActor2.pos[1] = 55.0f;
    usherKoboldActor2.pos[2] = 680.0f;
    usherKoboldActor2.rot[1] = T3D_DEG_TO_RAD(-90);
    animations_change(&usherKoboldActor2.anim, 2, 0.25f);

    //Dancefloor Kobolds
    koboldActor3.pos[0] = 160.0f;
    koboldActor3.pos[1] = 0.0f;
    koboldActor3.pos[2] = 0.0f;
    koboldActor3.rot[1] = T3D_DEG_TO_RAD(90);
    animations_change(&koboldActor3.anim, 2, 0.4f);

    koboldActor4.pos[0] = -160.0f;
    koboldActor4.pos[1] = 0.0f;
    koboldActor4.pos[2] = 0.0f;
    koboldActor4.rot[1] = T3D_DEG_TO_RAD(-90);
    animations_change(&koboldActor4.anim, 2, 0.3f);

    koboldActor5.pos[0] = 80.0f;
    koboldActor5.pos[1] = 0.0f;
    koboldActor5.pos[2] = 160.0f;
    koboldActor5.rot[1] = T3D_DEG_TO_RAD(-200);
    animations_change(&koboldActor5.anim, 2, 0.5f);

    koboldActor6.pos[0] = 80.0f;
    koboldActor6.pos[1] = 0.0f;
    koboldActor6.pos[2] = -160.0f;
    koboldActor6.rot[1] = T3D_DEG_TO_RAD(20);
    animations_change(&koboldActor6.anim, 2, 0.25f);

    koboldActor7.pos[0] = -80.0f;
    koboldActor7.pos[1] = 0.0f;
    koboldActor7.pos[2] = 160.0f;
    koboldActor7.rot[1] = T3D_DEG_TO_RAD(-160);
    animations_change(&koboldActor7.anim, 2, 0.35f);

    koboldActor8.pos[0] = -80.0f;
    koboldActor8.pos[1] = 0.0f;
    koboldActor8.pos[2] = -160.0f;
    koboldActor8.rot[1] = T3D_DEG_TO_RAD(340);
    animations_change(&koboldActor8.anim, 2, 0.45f);

    //Putting the dragon stuff here too
    dragonActor.scale[0] = 1.75f;
    dragonActor.scale[1] = 1.75f;
    dragonActor.scale[2] = 1.75f;


    actors[0] = dragonActor;
    actors[1] = stageActor;
    actors[2] = dynamoActor;
    actors[3] = koboldActor;
    actors[4] = koboldActor2;
    actors[5] = koboldActor3;
    actors[6] = koboldActor4;
    actors[7] = koboldActor5;
    actors[8] = koboldActor6;
    actors[9] = koboldActor7;
    actors[10] = koboldActor8;
    actors[11] = usherKoboldActor;
    actors[12] = usherKoboldActor2;

    // These are test values. You can look at them by pressing A for Dergs, B for Dynamo
    dergVector = actor_get_pos_vec(&dragonActor);
    dergVector.x += dragonActor.initialAabbMax[0] / 2;
    dergVector.y += dragonActor.initialAabbMax[1] / 2;
    dergVector.z += dragonActor.initialAabbMax[2] / 2;

    dynamoVector = actor_get_pos_vec(&dynamoActor);
    dergVector.x += dynamoActor.initialAabbMax[0] / 2;
    dergVector.y += dynamoActor.initialAabbMax[1] / 2;
    dergVector.z += dynamoActor.initialAabbMax[2] / 2;

    // Camera init
    camera.rotation.y = -0.48;
    camera.pos.x = -200.0f;
    camera.pos.y = 150.0f;
    camera.pos.z = -80.0f;

    camera_look_at(&camera, &dynamoVector);
    camera_update(&camera, &viewport, 0.0f);

    //Sprite loading for UI
    playBtnDownSprite = sprite_load("rom:/play-btn-down.sprite");
    playBtnUpSprite = sprite_load("rom:/play-btn-up.sprite");
    trackBackSprite = sprite_load("rom:/track-back.sprite");
    trackFwdSprite = sprite_load("rom:/track-fwd.sprite");
    transBG1 = sprite_load("rom:/TransBG1.sprite");
    dragonBackdrop =sprite_load("rom:/TestImageDragon3.sprite");

    return 1;
}

void stage_take_input(enum GameState passedGameState) {
    if (btnsPressed.start) {
        gameState = gameState == STAGE ? PAUSED : STAGE;
        for (int i=1;i<360;i++){
          int randintx = rand();
          int randinty = rand();
          return;
          }
    }

    if (passedGameState == STAGE) {
        // TIES up controls:
        // Analogue Stick, C up and Down, Z
        camera_take_input_debug(&camera, &viewport, deltaTime);
    }

    if (btnsPressed.d_up)  lightBehaviourIndex++;
    if (btnsPressed.d_down)  lightBehaviourIndex--;

    if (lightBehaviourIndex > 2) lightBehaviourIndex = 0;
    if (lightBehaviourIndex < 0) lightBehaviourIndex = 2;

}

void stage_render_frame(enum GameState passedGameState) {
    // ======== Update
    deltaTime = display_get_delta_time();
    spinTimer += deltaTime;
    horizAnimationTimer += deltaTime;
    vertAnimationTimer += deltaTime;

    if (spinTimer > RAD_360 * 4) spinTimer = 0;
    if (horizAnimationTimer > viewport.size[0] * 4) horizAnimationTimer = 0;
    if (vertAnimationTimer > viewport.size[1] * 4) vertAnimationTimer = 0;

    camera_update(&camera, &viewport, deltaTime);

    for (int i = 0; i < ACTORS_COUNT; i++) {
        Actor* curActor = &actors[i];
        if (passedGameState == STAGE) {
            actor_update(curActor, spinTimer, deltaTime);
        }
    }

    if (syncPoint)rspq_syncpoint_wait(syncPoint); // wait for the RSP to process the previous frame

    directionalLights[0].lightUpdateFunction = lightBehaviourArray[lightBehaviourIndex].updateFunction;

    for (int i = 0; i < DIRECTIONAL_LIGHT_COUNT; i++) {
        Light* curLight = &directionalLights[i];
        if (curLight->lightUpdateFunction) {
            curLight->lightUpdateFunction(curLight, deltaTime, spinTimer);
        }
    }
    // <Draw>
    t3d_draw_update(&viewport);

    // = <Inner Draw>
    t3d_matrix_push_pos(1);

    // = lights
    t3d_light_set_ambient(ambientLightColour);

    for (int i = 0; i < DIRECTIONAL_LIGHT_COUNT; i++) {
        light_draw(&directionalLights[i], i);
    }

    t3d_light_set_count(DIRECTIONAL_LIGHT_COUNT);

    for (int i = 0; i < ACTORS_COUNT; i++) {
        Actor* curActor = &actors[i];

        if (curActor->animCount) {
            t3d_skeleton_update(&curActor->anim.skel);
        }

        if (curActor->t3dModel) {
            check_aabbs(curActor);
        }

        // draw_aabbs(curActor);

        actor_draw(curActor, spinTimer);
    }
    t3d_matrix_pop(1);

    // = 2D
    syncPoint = rspq_syncpoint_new();
    rdpq_sync_pipe();

    regular_prints();
   // debug_prints();

    if (passedGameState == PAUSED) {

      rdpq_mode_push();
      rdpq_set_mode_standard();
      rdpq_mode_blender(RDPQ_BLENDER_ADDITIVE);
      rdpq_sprite_blit(dragonBackdrop,0,0,NULL);
      rdpq_mode_pop();

      text_box(
          "Non dolores est esse dolore. Ut quia dolorem id commodi dignissimos soluta. Dolore eum atque quia enim suscipit. Fuga repellendus quis soluta quia autem adipisci reiciendis veritatis. Molestiae debitis aliquid iste iusto.",
          *randx,
          *randy
          );

        rdpq_text_printf(
          &(rdpq_textparms_t){
              .width = display_get_width()-16,
              .height = display_get_height()-16,
              .align = ALIGN_CENTER,
              .valign = VALIGN_CENTER,
        }, 6, 16, 16, "You've pressed pause.\nPress START to unpause.");

    }

    rdpq_detach_show();
    // </Draw>
}


void stage_teardown() {
    for (int i = 0; i < ACTORS_COUNT; i++) {
        actor_delete(&actors[i]);
    }
    sprite_free(trackFwdSprite);
    sprite_free(trackBackSprite);
    sprite_free(playBtnDownSprite);
    sprite_free(playBtnUpSprite);
    sprite_free(dragonBackdrop);
    sprite_free(transBG1);
    //sprite_free(koboldShortTape);
}



// ==== PRIVATE ====


//Generate a random text box somewhere in the screen
void text_box(const char *text, int box_screenpos_x, int box_screenpos_y) {

  int	res_x = 320;
  int 	res_y = 240;
  int	box_width = 200;
  int	box_height = 50;
  int	border = 4;

  rdpq_set_mode_fill(RGBA32(128,50,128,255));
  rdpq_fill_rectangle(
      box_screenpos_x,
      box_screenpos_y,
      box_screenpos_x + box_width,
      box_screenpos_y + box_height
  );
  rdpq_text_printf(&(rdpq_textparms_t) {
      .width = box_width - border*2,
      .height = box_height - border*2,
      .align = ALIGN_LEFT,
      .valign = VALIGN_CENTER,
      .wrap = WRAP_WORD,
      .line_spacing = 1
      }, 4, box_screenpos_x + border, box_screenpos_y + border, text);

};

static void t3d_draw_update(T3DViewport *viewport) {
    rdpq_attach(display_get(), display_get_zbuf());
    t3d_frame_start();
    t3d_viewport_attach(viewport);

    rdpq_mode_combiner(RDPQ_COMBINER_SHADE);

    rdpq_mode_fog(RDPQ_FOG_STANDARD);
    rdpq_set_fog_color(fogColour);

    rdpq_mode_antialias(AA_NONE);
    rdpq_mode_alphacompare(1);

    rdpq_mode_mipmap(MIPMAP_NONE, 0);
    t3d_fog_set_range(fogNear, fogFar);
    t3d_fog_set_enabled(true);
    t3d_state_set_drawflags( T3D_FLAG_DEPTH | T3D_FLAG_SHADED );

    t3d_screen_clear_color(fogColour);
    t3d_screen_clear_depth();
}


static void check_aabbs(Actor *curActor) {
    T3DFrustum frustum = viewport.viewFrustum;

    curActor->visible = t3d_frustum_vs_aabb_s16(&frustum, curActor->t3dModel->aabbMin,
                                               curActor->t3dModel->aabbMax);

    if (curActor->visible) {

        T3DModelIter it = t3d_model_iter_create(curActor->t3dModel, T3D_CHUNK_TYPE_OBJECT);
        while (t3d_model_iter_next(&it)) {
            // Skip fine checks for animated models
            // It doesn't really work properly
            if (curActor->animCount == 0) {
                it.object->isVisible = true;
                continue;
            }

            int16_t transposedAabbMin[3];
            int16_t transposedAabbMax[3];
            aabb_mat4_mult(transposedAabbMin, it.object->aabbMin, curActor->modelMatF);
            aabb_mat4_mult(transposedAabbMax, it.object->aabbMax, curActor->modelMatF);

            it.object->isVisible =
                t3d_frustum_vs_aabb_s16(&frustum, transposedAabbMin, transposedAabbMax);
        }
    }
}




//static void sine_text(const char* text, float speedFactor, float xOffset, float yOffset, bool scroll ) {
//    int strLen = strlen(text);
//
//    int xScroll = scroll ? horizAnimationTimer * speedFactor : 0;
//
//    for (int i = 0; i < strLen; i++) {
//        rdpq_font_style(bitDotted, 0, &(rdpq_fontstyle_t){
//          .color = RGBA32(187,244,139,255),
//          .outline_color = RGBA32(0,0,0,255),
//          //hsla2rgba( 0.01f * spinTimer,fm_sinf(spinTimer + i),0.5f,1.0f),
//        });
//        rdpq_text_printn(
//        &(rdpq_textparms_t) {
//            .style_id = 0,
//        },
//        5,
//        fm_fmodf(xScroll  + (i * 12), display_get_width()) + xOffset,
//        (fm_sinf(horizAnimationTimer + i) * speedFactor) + yOffset,
//        &text[i], 1);
//    }
//
//}

static const int charHeight = 8;
static int margin = 8;
static int fpsPos = charHeight*2;
static int overScan = 0;

static void regular_prints() {
    int displayHeight = display_get_height();

    int musicTitlePos = displayHeight - charHeight*4 - overScan;
    int artistTitlePos = displayHeight - charHeight*6 - overScan;
    int tapePos = displayHeight - 26 - overScan;
    int btnPos = displayHeight - 40 - overScan;

    rdpq_mode_push();
    rdpq_set_mode_standard();
    rdpq_mode_blender(RDPQ_BLENDER_MULTIPLY);
    rdpq_sprite_blit(transBG1,0, btnPos, NULL);
    rdpq_set_mode_copy(true);
    rdpq_sprite_blit(trackBackSprite, 16, btnPos, NULL);
    rdpq_sprite_blit(trackFwdSprite,272, btnPos, NULL);
    //rdpq_set_mode_standard();
    //rdpq_mode_filter(FILTER_POINT);
    //rdpq_mode_alphacompare(1);                // colorkey (draw pixel with alpha >= 1)
    rdpq_mode_pop();

//   sine_text(xm.ctx->module.instruments[0].name, 2.0f, 56.0f ,  artistTitlePos, false);
//   sine_text(xm_get_module_name(xm.ctx), 2.0f, 56.0f ,  musicTitlePos, false);


    rdpq_text_printf(
        &(rdpq_textparms_t){
            .width = 320-margin,
            .height = 32,
            .align = ALIGN_RIGHT,
            .valign = VALIGN_TOP,
        }, 4, 0, margin, lightBehaviourArray[lightBehaviourIndex].name);
    rdpq_text_printf(
        &(rdpq_textparms_t){
            .width = 320,
            .height = 32,
            .align = ALIGN_LEFT,
            .valign = VALIGN_TOP,
        }, 4, margin, margin, "Speedicity: %.0f", display_get_fps());
    rdpq_text_printf(
        &(rdpq_textparms_t){
			.width = 320,
            .height = 32,
            .align = ALIGN_CENTER,
            .valign = VALIGN_TOP,
		}, 5, 0, 200, xm.ctx->module.instruments[0].name);
     rdpq_text_printf(
        &(rdpq_textparms_t){
			.width = 320,
            .height = 32,
            .align = ALIGN_CENTER,
            .valign = VALIGN_BOTTOM,
		}, 5, 0, 200, xm_get_module_name(xm.ctx));

}



//static void debug_prints() {
//    rdpq_text_printf(nullptr, 3, /*margin*/ 220.0f, fpsPos, "FPS: %.2f", display_get_fps());
//}

static void draw_aabbs(Actor* curActor) {
    uint16_t debugClr[4] = {0xFF, 0x00, 0x00, 0xFF};

    debugDrawAABB(display_get_current_framebuffer().buffer,
                    curActor->t3dModel->aabbMin,
                    curActor->t3dModel->aabbMax,
                  &viewport, 1.0f, debugClr[0]);
}



