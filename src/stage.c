
#include "../libs/libxm/xm.h"
#include "../libs/libxm/xm_internal.h"

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

#define ACTORS_COUNT 3
#define DIRECTIONAL_LIGHT_COUNT 2

static Actor actors[ACTORS_COUNT];
static Camera camera;
static bool _cameraDirty;

static float deltaTime = 0.0f;
static float fogNear = 100.0f;
static float fogFar = 250.0f;
static color_t fogColour = {70, 70, 140, 0xFF};

static float spinTimer = 0.0f;
static float horizAnimationTimer = 0.0f;
static float vertAnimationTimer = 0.0f;

static T3DViewport viewport;
static Light directionalLights[DIRECTIONAL_LIGHT_COUNT];
uint8_t ambientLightColour[4] = {80, 80, 80, 0x7f};
rspq_syncpoint_t syncPoint = 0;
rdpq_font_t* ftrFont;


static LightBehaviour lightBehaviourArray[4] = {
    {
    .name = "traffic light",
    .updateFunction = &light_update_traffic_light
    },
{
    .name = "synced traffic light",
    .updateFunction = &light_update_traffic_light_xm
    },
{
    .name = "synced tekno strobe",
    .updateFunction = &light_update_xm_tekno_strobe
    },
{
    .name = "mono volume follow",
    .updateFunction = &light_update_vol_follow
    },
};

static int lightBehaviourIndex = 0;

static void t3d_draw_update(T3DViewport *viewport);

static void debug_prints();
static void regular_prints();
static void draw_aabbs(Actor* curActor);

static void check_aabbs(Actor *curActor);
static void sine_text(const char* text, float speedFactor, float xOffset, float yOffset, bool scroll );

static sprite_t* playBtnUpSprite;
static sprite_t* playBtnDownSprite;
static sprite_t* trackBackSprite;
static sprite_t* trackFwdSprite;
static rspq_block_t* hudBlock;

// ==== PUBLIC ====
int stage_setup() {
    viewport = t3d_viewport_create();

    camera = camera_create();

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

    dynamo_init();

    dragonActor.updateFunction = &dragon_update;
    dynamoActor.customPartDrawFunc = &dynamo_part_draw;

    dynamoActor.pos[0] = -330.0f;
    dynamoActor.pos[1] = 30.0f;
    dynamoActor.pos[2] = -30.0f;

    dragonActor.scale[0] = 2.0f;
    dragonActor.scale[1] = 2.0f;
    dragonActor.scale[2] = 2.0f;

    dynamoActor.rot[1] = T3D_DEG_TO_RAD(-90);

    actors[0] = dragonActor;
    actors[1] = stageActor;
    actors[2] = dynamoActor;

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

    playBtnDownSprite = sprite_load("rom:/play-btn-down.sprite");
    playBtnUpSprite = sprite_load("rom:/play-btn-up.sprite");
    trackBackSprite = sprite_load("rom:/track-back.sprite");
    trackFwdSprite = sprite_load("rom:/track-fwd.sprite");

    return 1;
}

void stage_take_input(enum GameSate passedGameState) {
    if (btnsPressed.start) {
        gameState = gameState == STAGE ? PAUSED : STAGE;
    }

    if (passedGameState == STAGE) {
        // TIES up controls:
        // Analogue Stick, C up and Down, Z
        camera_take_input_debug(&camera, &viewport, deltaTime);
    }

    if (btnsPressed.d_up)  lightBehaviourIndex++;
    if (btnsPressed.d_down)  lightBehaviourIndex--;

    if (lightBehaviourIndex > 3) lightBehaviourIndex = 0;
    if (lightBehaviourIndex < 0) lightBehaviourIndex = 3;

}

void stage_render_frame(enum GameSate passedGameState) {
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
    debug_prints();

    if (passedGameState == PAUSED) {
        sine_text("PAUSED!", 16.0f, 112.0f, 96.0f, false );
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
}

// ==== PRIVATE ====

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

static constexpr int charHeight = 16;
static constexpr int margin = 32;
static constexpr int fpsPos = charHeight*2;

static void regular_prints() {
     int musicTitlePos = display_get_height() - charHeight*4;
     int artistTitlePos = display_get_height() - charHeight*5;
    rdpq_set_mode_copy(true);
    rdpq_mode_push();
    rdpq_mode_tlut(TLUT_RGBA16);
    //rspq_block_run(hudBlock);
    switch (gameState) {
        default:
        case STAGE:
            rdpq_sprite_blit(playBtnDownSprite, margin + 64, 20, nullptr);
            break;
        case PAUSED:
            rdpq_sprite_blit(playBtnUpSprite, margin + 64, 20, nullptr);
            break;
    }


    rdpq_sprite_blit(trackBackSprite, 32, musicTitlePos-16, nullptr );
    rdpq_sprite_blit(trackFwdSprite,260 , musicTitlePos-16, nullptr );
    rdpq_set_mode_standard();
    rdpq_mode_filter(FILTER_BILINEAR);
    rdpq_mode_alphacompare(1);                // colorkey (draw pixel with alpha >= 1)
    rdpq_mode_pop();

    sine_text(xm.ctx->module.instruments[0].name, 4.0f, 70.0f ,  artistTitlePos, false);
    sine_text(xm_get_module_name(xm.ctx), 4.0f, 70.0f ,  musicTitlePos, false);
    rdpq_text_printf(nullptr, 3, 220.0f ,  fpsPos, lightBehaviourArray[lightBehaviourIndex].name);
}



static void debug_prints() {
    rdpq_text_printf(nullptr, 3, margin, fpsPos, "FPS: %.2f", display_get_fps());
}

static void draw_aabbs(Actor* curActor) {
    uint16_t debugClr[4] = {0xFF, 0x00, 0x00, 0xFF};

    debugDrawAABB(display_get_current_framebuffer().buffer,
                    curActor->t3dModel->aabbMin,
                    curActor->t3dModel->aabbMax,
                  &viewport, 1.0f, debugClr[0]);
}



