#include "actor.h"
#include "camera.h"
#include "actorBehaviours.h"
#include "globals.h"
#include "lights.h"
#include "debugDraw.h"
#include "aabbHelpers.h"
#include "libs/libxm/xm.h"

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
T3DVec3 camPosScreen;
sprite_t* dynamoEyeTex[4];

static inline void t3d_draw_update(T3DViewport *viewport);

constexpr char magicString[32] = "rom:/MainBarArea.t3dm";



int stage_setup() {
    viewport = t3d_viewport_create();

    camera = camera_create();

    // ======== Init Lights
    uint8_t colorDir[4] = {0xFF, 0x44, 0x44, 0xFF};
    T3DVec3 lightDirVec = {{0.0f, 1.0f, 0.0f}};
    Light pointLightOne = light_create(colorDir, lightDirVec, false);

    uint8_t colorDir2[4] = {0xFF, 0x00, 0x00, 0xFF};
    T3DVec3 lightDirVec2 = {{-1.0f, 1.0f, -1.0f}};
    Light pointLightTwo = light_create(colorDir2, lightDirVec2, true);

    uint8_t colorDir3[4] = {0x00, 0x00, 0xFF, 0xFF};
    T3DVec3 lightDirVec3 = {{1.0f, 1.0f, 1.0f}};
    Light pointLightThree = light_create(colorDir3, lightDirVec3, false);

    directionalLights[0] = pointLightOne;
     directionalLights[1] = pointLightTwo;
    directionalLights[2] = pointLightThree;

    // ======== Init Actors

    Actor dragonActor = create_actor_from_model("dragon2");
    Actor stageActor = create_actor_from_model("MainBarArea");
    Actor dynamoActor = create_actor_from_model("DynamoAnimation");

    sprite_t* dynamoEyeTex[4] = {
        sprite_load("rom:/EYE-DYNAMO1.sprite"),
        sprite_load("rom:/EYE-DYNAMO-CLOSE.sprite"),
        sprite_load("rom:/EYE-DYNAMO-DOWN.sprite"),
        sprite_load("rom:/EYE-DYNAMO-UP.sprite"),
    };

    actor_attach_update_function(&dragonActor, &dragon_update);

    // dynamoActor.customPartDrawFunc = &dynamo_part_draw;

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
    dergVector = (T3DVec3){
        {
            dragonActor.pos[0] + dragonActor.initialAabbMax[0] / 2,
            dragonActor.pos[1] + dragonActor.initialAabbMax[1] / 2,
            dragonActor.pos[2] + dragonActor.initialAabbMax[2] / 2
        }
    };

    dynamoVector = (T3DVec3){
        {
            dynamoActor.pos[0] + dynamoActor.initialAabbMax[0] / 2,
            dynamoActor.pos[1] + dynamoActor.initialAabbMax[1] / 2,
            dynamoActor.pos[2] + dynamoActor.initialAabbMax[2] / 2
        }
    };

    camera.rotation.x = 2.68;
    camera.pos.x = 80.0f;
    camera.pos.y = 80.0f;
    camera.pos.z = -80.0f;

    // camera_look_at(&camera, &dergVector, &viewport);
    camera_update(&camera, &viewport, objTime);

    return 1;
}

float modelScale = 1.0f;

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

void stage_loop(int running) {
    // ======== Update

    // pause when button is released
    if (btnsPressed.start) {
        gameState = gameState == STAGE ? PAUSED : STAGE;
    }

    float deltaTime = display_get_delta_time(); // (newTime - objTimeLast) * baseSpeed;
    objTime += deltaTime;

    if (objTime == MAXFLOAT) objTime = 0;

    // TIES up controls:
    // Analogue Stick, C up and Down, Z
    if (running) {
        camera_update(&camera, &viewport, deltaTime);
    }

    if (inputs.btn.d_up) fogNear--;
    if (inputs.btn.d_down) fogNear++;

    if (inputs.btn.d_left) fogFar --;
    if (inputs.btn.d_right) fogFar ++;

    for (int i = 0; i < ACTORS_COUNT; i++) {
        Actor* curActor = &actors[i];
        if (running) {
            actor_update(curActor, objTime, deltaTime);
        }
    }

    if (syncPoint)rspq_syncpoint_wait(syncPoint); // wait for the RSP to process the previous frame

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

        if (curActor->anim.animationCount) {
            t3d_skeleton_update(&curActor->anim.skel);
        }


        if (curActor->t3dModel) {
            check_aabbs(curActor);
        }

       // uint16_t debugClr[4] = {0xFF, 0x00, 0x00, 0xFF};

        // debugDrawAABB(display_get_current_framebuffer().buffer,
        //                 curActor->t3dModel->aabbMin,
        //                 curActor->t3dModel->aabbMax,
        //               &viewport, 1.0f, debugClr[0]);

        actor_draw(curActor, objTime);
    }
    t3d_matrix_pop(1);
    // = </Inner Draw>

    // = 2D
    syncPoint = rspq_syncpoint_new();
    rdpq_sync_pipe();


    // u_int16_t bpm;
    // u_int16_t tempo;

    // camera_draw(); // purely for debug
    rdpq_text_printf(nullptr, FONT_BUILTIN_DEBUG_MONO, 16, 16, "FPS: %.2f", display_get_fps());
    // rdpq_text_printf(nullptr, FONT_BUILTIN_DEBUG_MONO, 16, 32, "playing song: %s", xm_get_module_name(xm.ctx));
    // rdpq_text_printf(nullptr, FONT_BUILTIN_DEBUG_MONO, 16, 48, "camera rot: %.2f, %.2f, %.2f", camera.rotation.x, camera.rotation.y, camera.rotation.z );
    // // rdpq_text_printf(nullptr, FONT_BUILTIN_DEBUG_MONO, 16, 64, "tpl: %i", tempo);

    // ===== Audio
    if (running) {
        mixer_try_play();
    } else {
        rdpq_text_printf(nullptr, FONT_BUILTIN_DEBUG_MONO, 120, 96, "PAUSED");
    }

    rdpq_detach_show();
    // </Draw>
}

void stage_teardown() {
    for (int i = 0; i < ACTORS_COUNT; i++) {
        actor_delete(&actors[i]);
    }
}

static inline void t3d_draw_update(T3DViewport *viewport) {
    rdpq_attach(display_get(), display_get_zbuf());
  //  rdpq_mode_zbuf(true, true);
    t3d_frame_start();
    t3d_viewport_attach(viewport);

    rdpq_mode_combiner(RDPQ_COMBINER_SHADE);

    rdpq_mode_fog(RDPQ_FOG_STANDARD);
    rdpq_set_fog_color(fogColour);

    rdpq_mode_antialias(AA_NONE);

    rdpq_mode_mipmap(MIPMAP_NONE, 0);
    t3d_fog_set_range(fogNear, fogFar);
    t3d_fog_set_enabled(true);
    t3d_state_set_drawflags( T3D_FLAG_DEPTH | T3D_FLAG_SHADED );

    t3d_screen_clear_color(fogColour);
    t3d_screen_clear_depth();
}
