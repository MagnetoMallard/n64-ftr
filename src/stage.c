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
#include "dialogue.h"
#include "stage_data.h"

static float deltaTime = 0.0f;
static float fogNear = 150.0f;
static float fogFar = 300.0f;
//static color_t fogColour = {70, 70, 70, 0xFF};
static color_t fogColour = {87, 110, 168, 0xFF}; //nice colour :)
//static color_t fogColour = {163, 142, 103, 0xFF};

static float spinTimer = 0.0f;
static float horizAnimationTimer = 0.0f;
static float vertAnimationTimer = 0.0f;

static T3DViewport viewport;
uint8_t ambientLightColour[4] = {100, 100, 100, 0x7f};
rspq_syncpoint_t syncPoint = 0;
rdpq_font_t* ftrFont;
rdpq_font_t* V5PRC___;
rdpq_font_t* ftrFontSkinny;
rdpq_font_t* ftrFontBig;
surface_t* disp;

static int boxWidth = 140;
static int boxHeight = 38;
static int textBoxPosX;
static int textBoxPosY;

int textSegment = 1;
int koboldDialogueCounter = 0; //for the switch statement

static int lightBehaviourIndex = 0;

static void t3d_draw_update(T3DViewport* viewport);
static void regular_prints();
static void check_aabbs(Actor* curActor);

static void debug_prints();
static void draw_aabbs(Actor* curActor);

//static void text_box(const char* textCont, const char* textName, int boxScreenposX, int boxScreenposY);
//static void sine_text(const char* text, float speedFactor, float xOffset, float yOffset, bool scroll );

static Camera camera;
static StageData stageData;

// ==== PUBLIC ====
int stage_setup() {

    viewport = t3d_viewport_create();

    camera = camera_create();

    // ======== Init Lights
    initStageLights(&stageData);

    // ======== Init Actors
    initStageActors(&stageData);

    // ======== Init Sprites
    initStageSprites(&stageData);

    textBoxPosX = rand() % ((display_get_width() - boxWidth));
    textBoxPosY = rand() % ((display_get_height() - boxHeight));

    // These are test values. You can look at them by pressing A for Dergs, B for Dynamo
    dergVector = actor_get_pos_vec(&stageData.actors[0]);
    dergVector.x += stageData.actors[0].initialAabbMax[0] / 2;
    dergVector.y += stageData.actors[0].initialAabbMax[1] / 2;
    dergVector.z += stageData.actors[0].initialAabbMax[2] / 2;

    dynamoVector = actor_get_pos_vec(&stageData.actors[2]);
    dergVector.x += stageData.actors[2].initialAabbMax[0] / 2;
    dergVector.y += stageData.actors[2].initialAabbMax[1] / 2;
    dergVector.z += stageData.actors[2].initialAabbMax[2] / 2;

    // Camera init
    camera.rotation.y = -0.48;
    camera.pos.x = -200.0f;
    camera.pos.y = 150.0f;
    camera.pos.z = -80.0f;

    camera_look_at(&camera, &dynamoVector);
    camera_update(&camera, &viewport, 0.0f);

    return 1;
};

//---DONE WITH THE TEXT NOW--


void stage_take_input(enum GameState passedGameState) {
    if (btnsPressed.start) {
        koboldDialogueCounter = 0;
        gameState = gameState == STAGE ? PAUSED : STAGE;
        textBoxPosX = rand() % ((display_get_width() - boxWidth));
        textBoxPosY = rand() % ((display_get_height() - boxHeight));
    }

    if (passedGameState == STAGE) {
        // TIES up controls:
        // Analogue Stick, C up and Down, Z
        camera_take_input_debug(&camera, &viewport, deltaTime);
    }

    if (passedGameState == PAUSED) {
        if (btnsUp.a) koboldDialogueCounter++;
    }

    if (btnsPressed.d_up) lightBehaviourIndex++;
    if (btnsPressed.d_down) lightBehaviourIndex--;

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
        Actor* curActor = &stageData.actors[i];
        if (passedGameState == STAGE) {
            actor_update(curActor, spinTimer, deltaTime);
        }
    }

    if (syncPoint)rspq_syncpoint_wait(syncPoint); // wait for the RSP to process the previous frame

    stageData.directionalLights[0].lightUpdateFunction = stageData.lightBehaviours[lightBehaviourIndex].updateFunction;

    for (int i = 0; i < DIRECTIONAL_LIGHT_COUNT; i++) {
        Light* curLight = &stageData.directionalLights[i];
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
        light_draw(&stageData.directionalLights[i], i);
    }

    t3d_light_set_count(DIRECTIONAL_LIGHT_COUNT);

    for (int i = 0; i < ACTORS_COUNT; i++) {
        Actor* curActor = &stageData.actors[i];

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
        rdpq_sprite_blit(stageData.dragonBackdrop, 0, 0,NULL);
        rdpq_mode_pop();

        rdpq_text_printf(
            &(rdpq_textparms_t){
                .width = display_get_width() - 16,
                .height = display_get_height() - 16,
                .align = ALIGN_CENTER,
                .valign = VALIGN_CENTER,
            },
            6,
            16,
            16,
            "You've pressed pause.\nPress START to unpause."
        );

        TextBoxParams koboldTexBox = {
            .textBoxContent = &kob[koboldDialogueCounter],
            .boxScreenPosX = textBoxPosX,
            .boxScreenPosY = textBoxPosY,
            .boxWidth = boxWidth,
            .boxHeight = boxHeight,
        };

        if (koboldDialogueCounter < 3) {
            text_box(&koboldTexBox);
        }
    }

    rdpq_detach_show();
    // </Draw>
}


void stage_teardown() {
    for (int i = 0; i < ACTORS_COUNT; i++) {
        actor_delete(&stageData.actors[i]);
    }
    sprite_free(stageData.trackFwdSprite);
    sprite_free(stageData.trackBackSprite);
    sprite_free(stageData.playBtnDownSprite);
    sprite_free(stageData.playBtnUpSprite);
    sprite_free(stageData.dragonBackdrop);
    sprite_free(stageData.transBG1);
    // sprite_free(koboldShortTape);
}


// ==== PRIVATE ====


static void t3d_draw_update(T3DViewport* viewport) {
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
    t3d_state_set_drawflags(T3D_FLAG_DEPTH | T3D_FLAG_SHADED);

    t3d_screen_clear_color(fogColour);
    t3d_screen_clear_depth();
}


static void check_aabbs(Actor* curActor) {
    T3DFrustum frustum = viewport.viewFrustum;

    curActor->visible = t3d_frustum_vs_aabb_s16(&frustum,
                                                curActor->t3dModel->aabbMin,
                                                curActor->t3dModel->aabbMax
    );

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



static int margin = 8;
static int overScan = 0;

static void regular_prints() {
    int displayHeight = display_get_height();
    int btnPos = displayHeight - 40 - overScan;


    rdpq_mode_push();
    rdpq_set_mode_standard();
    rdpq_mode_blender(RDPQ_BLENDER_MULTIPLY);
//    rdpq_sprite_blit(transBG1, 0, btnPos, NULL);
//    rdpq_set_mode_copy(true);

    rdpq_sprite_blit(stageData.trackBackSprite, 16, btnPos, NULL);
    rdpq_sprite_blit(stageData.trackFwdSprite, 272, btnPos, NULL);
    rdpq_mode_pop();


    rdpq_text_printf(
        &(rdpq_textparms_t){
            .width = 320 - margin,
            .height = 32,
            .align = ALIGN_RIGHT,
            .valign = VALIGN_TOP,
        },
        4,
        0,
        margin,
        stageData.lightBehaviours[lightBehaviourIndex].name
    );
    rdpq_text_printf(
        &(rdpq_textparms_t){
            .width = 320,
            .height = 32,
            .align = ALIGN_LEFT,
            .valign = VALIGN_TOP,
        },
        4,
        margin,
        margin,
        "Speedicity: %.0f",
        display_get_fps()
    );
    rdpq_text_printf(
        &(rdpq_textparms_t){
            .width = 320,
            .height = 32,
            .align = ALIGN_CENTER,
            .valign = VALIGN_TOP,
        },
        5,
        0,
        200,
        xm.ctx->module.instruments[0].name
    );
    rdpq_text_printf(
        &(rdpq_textparms_t){
            .width = 320,
            .height = 32,
            .align = ALIGN_CENTER,
            .valign = VALIGN_BOTTOM,
        },
        5,
        0,
        200,
        xm_get_module_name(xm.ctx)
    );
}

static void sine_text(const char* text, float speedFactor, float xOffset, float yOffset, bool scroll) {
    unsigned int strLen = strlen(text);

    int xScroll = scroll ? horizAnimationTimer * speedFactor : 0;

    for (int i = 0; i < strLen; i++) {
        rdpq_font_style(ftrFontBig,
                        0,
                        &(rdpq_fontstyle_t){
                            .color = RGBA32(187, 244, 139, 255),
                            .outline_color = RGBA32(0, 0, 0, 255),
                            //hsla2rgba( 0.01f * spinTimer,fm_sinf(spinTimer + i),0.5f,1.0f),
                        }
        );
        rdpq_text_printn(
            &(rdpq_textparms_t){
                .style_id = 0,
            },
            5,
            fm_fmodf(xScroll + (i * 12), display_get_width()) + xOffset,
            (fm_sinf(horizAnimationTimer + i) * speedFactor) + yOffset,
            &text[i],
            1
        );
    }
}

static void draw_aabbs(Actor* curActor) {
    uint16_t debugClr[4] = {0xFF, 0x00, 0x00, 0xFF};

    debugDrawAABB(display_get_current_framebuffer().buffer,
                  curActor->t3dModel->aabbMin,
                  curActor->t3dModel->aabbMax,
                  &viewport,
                  1.0f,
                  debugClr[0]
    );
}
