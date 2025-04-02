#include "actor.h"
#include "camera.h"
#include "dragon.h"
#include "globals.h"
#include "lights.h"

#include "stage.h"

#define ACTORS_COUNT 3
#define DIRECTIONAL_LIGHT_COUNT 3

static Actor actors[ACTORS_COUNT];
static Camera camera;

static float fogNear = 100.0f;
static float fogFar = 250.0f;
static color_t fogColour = {70, 70, 140, 0xFF};

static float objTime = 0.0f;
static T3DViewport viewport;
static Light directionalLights[DIRECTIONAL_LIGHT_COUNT];
uint8_t ambientLightColour[4] = {80, 80, 80, 0xFF};
rspq_syncpoint_t syncPoint = 0;

static inline void t3d_draw_update(T3DViewport* viewport);

int stage_setup() {

    viewport = t3d_viewport_create();

    camera = camera_create();

    // ======== Init Lights
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

    Actor dragonActor = dragon_create();
    Actor stageActor = create_actor_from_model("MainBarArea");
    Actor dynamoActor = create_actor_from_model("Dynamo5");

    stageActor.scale[0] = 1.0f;
    stageActor.scale[1] = 0.8f;
    stageActor.scale[2] = 1.0f;

    dragonActor.pos[0] = -160.0f;
    dragonActor.pos[1] = 0.0f;
    dragonActor.pos[2] = 0.0f;

    dynamoActor.pos[0] = -10.0f;
    dynamoActor.pos[1] = 0.0f;
    dynamoActor.pos[2] = 100.0f;

    dynamoActor.rot[1] = -10.0f;


    actors[0] = dragonActor;
    actors[1] = stageActor;
    actors[2] = dynamoActor;

    dergVector = (T3DVec3){
        {
            dragonActor.pos[0],
            dragonActor.pos[1],
            dragonActor.pos[2]
        }};

    dynamoVector = (T3DVec3){
        {
            dynamoActor.pos[0],
            dynamoActor.pos[1],
            dynamoActor.pos[2]
    }};
    camera_update(&camera, &viewport);

    camera_look_at(&camera,&dergVector,&viewport);

    return 1;
}

void stage_loop(int running) {
    // ======== Update
    if (inputs.btn.start) gameState = gameState == STAGE ? PAUSED : STAGE;

    if (running) {

        float deltaTime = display_get_delta_time(); // (newTime - objTimeLast) * baseSpeed;
        objTime += deltaTime;

        // TIES up controls:
        // Analogue Stick, C up and Down, Z
        camera_update(&camera, &viewport);
        if(inputs.btn.d_up) fogNear--;
        if(inputs.btn.d_down) fogNear++;

        if(inputs.btn.d_left) fogFar--;
        if(inputs.btn.d_right) fogFar++;

        for(int i = 0; i < 3; i++) {
            actor_update(&actors[i], objTime);
        }
    }


    if(syncPoint)rspq_syncpoint_wait(syncPoint); // wait for the RSP to process the previous frame

    for(int i = 0; i < ACTORS_COUNT; i++) {
        if (actors[i].anim.animationCount) {
            t3d_skeleton_update(&actors[i].anim.skel);
            t3d_mat4fp_from_srt_euler(actors[i].modelMat, actors[i].scale, actors[i].rot, actors[i].pos);
        }
    }

    // <Draw>
    t3d_draw_update(&viewport);

    // = lights
    t3d_light_set_ambient(ambientLightColour);

    for(int i = 0; i < DIRECTIONAL_LIGHT_COUNT; i++) {
        light_draw(&directionalLights[i], i);
    }

    t3d_light_set_count(DIRECTIONAL_LIGHT_COUNT);

    // = <Inner Draw>
    for(int i = 0; i < ACTORS_COUNT; i++) {
        actor_draw(&actors[i]);
    }
    // = </Inner Draw>

    // = 2D
    syncPoint = rspq_syncpoint_new();
    rdpq_sync_pipe();

    camera_draw(); // purely for debug
    rdpq_text_printf(nullptr, FONT_BUILTIN_DEBUG_MONO, 16, 32, "FOG NEAR: %.2f",  fogNear);
    rdpq_text_printf(nullptr, FONT_BUILTIN_DEBUG_MONO, 16, 48, "FOG FAR: %.2f",  fogFar);
    rdpq_text_printf(nullptr, FONT_BUILTIN_DEBUG_MONO, 16, 64, "CAMERA XPOS: %.2f",  camera.pos.x);
    rdpq_text_printf(nullptr, FONT_BUILTIN_DEBUG_MONO, 16, 80, "CAMERA ZPOS: %.2f",  camera.pos.z);
    rdpq_text_printf(nullptr, FONT_BUILTIN_DEBUG_MONO, 16, 96, "FPS: %.2f",  display_get_fps());
    rdpq_detach_show();
    // </Draw>

    // ===== Audio
    if (running) {
        mixer_try_play();
    } else {
        rdpq_text_printf(nullptr, FONT_BUILTIN_DEBUG_MONO, 120, 96, "PAUSED");
    }
}

void stage_teardown() {
    for(int i = 0; i < ACTORS_COUNT; i++) {
        actor_delete(&actors[i]);
    }
}

static inline void t3d_draw_update(T3DViewport* viewport) {
    rdpq_attach(display_get(), display_get_zbuf());
    t3d_frame_start();
    t3d_viewport_attach(viewport);
    rdpq_mode_combiner(RDPQ_COMBINER_SHADE);

    rdpq_mode_fog(RDPQ_FOG_STANDARD);
    rdpq_set_fog_color(fogColour);

    t3d_screen_clear_color(RGBA32(70,70, 140, 0xFF));
    t3d_screen_clear_depth();

    rdpq_mode_filter(FILTER_BILINEAR);
    t3d_fog_set_range(fogNear, fogFar);
    t3d_fog_set_enabled(true);
    t3d_state_set_drawflags(T3D_FLAG_SHADED | T3D_FLAG_DEPTH );
}
