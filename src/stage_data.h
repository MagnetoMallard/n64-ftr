//
// Created by mallard on 05/09/2025.
//

#ifndef STAGE_DATA_H
#define STAGE_DATA_H

#include "actors/actor.h"
#include "actors/actor_behaviours.h"

#include "camera/camera.h"
#include "lights/light_behaviours.h"

#include <t3d/t3d.h>
#include <libdragon.h>

#define ACTORS_COUNT 15
#define DIRECTIONAL_LIGHT_COUNT 2

typedef struct _stage_data {
    Actor actors[ACTORS_COUNT];
    LightBehaviour lightBehaviours[3];
    Light directionalLights[2];
    // Camera camera[4]; TODO: Camera Stack Goes Here
    // sprite_t sprite[8]; TODO: Make use of this when we get hashmaps in

    sprite_t* playBtnUpSprite;
    sprite_t* playBtnDownSprite;
    sprite_t* trackBackSprite;
    sprite_t* trackFwdSprite;
    sprite_t* dragonBackdrop;
    sprite_t* transBG1;
} StageData;

void initStageSprites(StageData* stageData) {
    stageData->playBtnDownSprite = sprite_load("rom:/play-btn-down.sprite");
    stageData->playBtnUpSprite = sprite_load("rom:/play-btn-up.sprite");
    stageData->trackBackSprite = sprite_load("rom:/track-back.sprite");
    stageData->trackFwdSprite = sprite_load("rom:/track-fwd.sprite");
    stageData->transBG1 = sprite_load("rom:/TransBG1.sprite");
    stageData->dragonBackdrop = sprite_load("rom:/TestImageDragon3.sprite");
}

void initStageLights(StageData* stageData) {
    LightBehaviour lightBehaviourArray[3] = {
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

    memcpy(stageData->lightBehaviours, lightBehaviourArray, sizeof(lightBehaviourArray));

    uint8_t colorDir[4] = {0x00, 0x00, 0x00, 0xFF};
    T3DVec3 lightDirVec = {{0.0f, 1.0f, 0.0f}};
    Light pointLightOne = light_create(colorDir, lightDirVec, false);
    pointLightOne.lightUpdateFunction = lightBehaviourArray[0].updateFunction;

    uint8_t colorDir2[4] = {0xFF, 0x44, 0x44, 0xFF};
    T3DVec3 lightDirVec2 = {{0.0f, 0.0f, 1.0f}};
    Light pointLightTwo = light_create(colorDir2, lightDirVec2, false);

    stageData->directionalLights[0] = pointLightOne;
    stageData->directionalLights[1] = pointLightTwo;
}

void initStageActors(StageData* stageData) {
    Actor dragonActor = create_actor_from_model("dragon2");

    Actor stageActor = create_actor_from_model("MainBarArea");

    Actor dynamoActor = create_actor_from_model("DynamoAnimationToontown");
    // Actor grifActor = create_actor_from_model("grif");

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
    Actor koboldActor9 = create_actor_from_model("KoboldWithAnims");
    Actor SluttyGenetActor = create_actor_from_model("SluttyGenet");

    // setup dynamo's blink cycle
    dynamo_init();

    //---
    //Colour change of the kobolds, RGBA. Final alpha seems to be unnecessary.
    //Set kobold model's vertex colours to white.
    //---

    //Colour Stage Kobolds
    kobold_init(&koboldActor, (color_t){255, 255, 255});
    kobold_init(&koboldActor2, (color_t){255, 100, 100});

    //Colour Usher Kobolds
    kobold_init(&usherKoboldActor, (color_t){255, 200, 255});
    kobold_init(&usherKoboldActor2, (color_t){255, 255, 200});

    //Colour Dancefloor kobolds
    kobold_init(&koboldActor3, (color_t){0, 100, 100});
    kobold_init(&koboldActor4, (color_t){100, 100, 255});
    kobold_init(&koboldActor5, (color_t){100, 255, 100});
    kobold_init(&koboldActor6, (color_t){255, 100, 100});
    kobold_init(&koboldActor7, (color_t){100, 0, 100});
    kobold_init(&koboldActor8, (color_t){100, 200, 100});
    kobold_init(&koboldActor9, (color_t){100, 100, 200});

    dragonActor.updateFunction = &dragon_update;
    koboldActor2.updateFunction = &dragon_update;
    koboldActor9.updateFunction = &dragon_update;
    dynamoActor.customPartDrawFunc = &dynamo_part_draw;

    //START POSITIONS, ROTATION, AND ANIM
    //Start postions of actors as a vec3, 0 on all vecs are centred. .pos[1] is equal to Blender's Z direction (Y here?)
    //Models start faced towards the entrance, there's be a better way to represent this later I'm sure.
    T3DModelIter it = t3d_model_iter_create(stageActor.t3dModel, T3D_CHUNK_TYPE_OBJECT);
    while (t3d_model_iter_next(&it)) {
        if (strcmp(it.object->material->name,"BLANK")) {
            if (strcmp(it.object->name, "EmptyDJ")) {
                dynamoActor.pos[0] =  it.object->_padding[0];
                dynamoActor.pos[1] =  it.object->aabbMin[1];
                dynamoActor.pos[2] =  it.object->aabbMin[2];
            }
        }
    }
    //DJ
    // dynamoActor.pos[0] = -340.0f;
    // dynamoActor.pos[1] = 50.0f;
    // dynamoActor.pos[2] = -100.0f;
    dynamoActor.rot[1] = T3D_DEG_TO_RAD(-90);

    //Grif
//    grifActor.pos[0] = 0.0f;
//    grifActor.pos[1] = 0.0f;
//    grifActor.pos[2] = -100.0f;
//    grifActor.scale[0] = .25f;
//    grifActor.scale[1] = .25f;
//    grifActor.scale[2] = .25f;
//    animations_change(&grifActor.anim, 1, 0.3f);

    //Stage kobolds
    koboldActor.pos[0] = -284.0f;
    koboldActor.pos[1] = 10.0f;
    koboldActor.pos[2] = 100.0f;
    koboldActor.rot[1] = T3D_DEG_TO_RAD(-90);
    animations_change(&koboldActor.anim, 0, 0.3f);

    koboldActor2.pos[0] = -260.0f;
    koboldActor2.pos[1] = 10.0f;
    koboldActor2.pos[2] = -230.0f;
    koboldActor2.rot[1] = T3D_DEG_TO_RAD(-90);
    animations_change(&koboldActor2.anim, 0, 0.8f);

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
    animations_change(&koboldActor3.anim, 0, 0.4f);

    koboldActor4.pos[0] = -160.0f;
    koboldActor4.pos[1] = 0.0f;
    koboldActor4.pos[2] = 0.0f;
    koboldActor4.rot[1] = T3D_DEG_TO_RAD(-90);
    animations_change(&koboldActor4.anim, 0, 0.3f);

    koboldActor5.pos[0] = 80.0f;
    koboldActor5.pos[1] = 0.0f;
    koboldActor5.pos[2] = 160.0f;
    koboldActor5.rot[1] = T3D_DEG_TO_RAD(-200);
    animations_change(&koboldActor5.anim, 0, 0.5f);

    koboldActor6.pos[0] = 80.0f;
    koboldActor6.pos[1] = 0.0f;
    koboldActor6.pos[2] = -160.0f;
    koboldActor6.rot[1] = T3D_DEG_TO_RAD(20);
    animations_change(&koboldActor6.anim, 0, 0.25f);

    koboldActor7.pos[0] = -80.0f;
    koboldActor7.pos[1] = 0.0f;
    koboldActor7.pos[2] = 160.0f;
    koboldActor7.rot[1] = T3D_DEG_TO_RAD(-160);
    animations_change(&koboldActor7.anim, 0, 0.35f);

    koboldActor8.pos[0] = 510.0f;
    koboldActor8.pos[1] = 56.0f;
    koboldActor8.pos[2] = -5.0f;
    koboldActor8.rot[1] = T3D_DEG_TO_RAD(-90);
    animations_change(&koboldActor8.anim, 3, 0.35f);

    koboldActor9.pos[0] = 530.0f;
    koboldActor9.pos[1] = 56.0f;
    koboldActor9.pos[2] = 90.0f;
    koboldActor9.rot[1] = T3D_DEG_TO_RAD(-90);
    animations_change(&koboldActor9.anim, 4, 0.35f);

    //Camash
    //1=stand
    //2=shake
    //3=walk
    //4=passdrink
    //5=shrug
    //6=dance
    SluttyGenetActor.pos[0] = 450.0f;
    SluttyGenetActor.pos[1] = 0.0f;
    SluttyGenetActor.pos[2] = 200.0f;
    SluttyGenetActor.scale[0] = 1.30f;
    SluttyGenetActor.scale[1] = 1.30f;
    SluttyGenetActor.scale[2] = 1.30f;
    SluttyGenetActor.rot[1] = T3D_DEG_TO_RAD(90);

    //Putting the dragon stuff here too
    dragonActor.scale[0] = .30f;
    dragonActor.scale[1] = .30f;
    dragonActor.scale[2] = .30f;
    animations_change(&dragonActor.anim, 1, 1.0f);

    stageData->actors[0] = dragonActor;
    stageData->actors[1] = stageActor;
    stageData->actors[2] = dynamoActor;
    stageData->actors[3] = koboldActor;
    stageData->actors[4] = koboldActor2;
    stageData->actors[5] = koboldActor3;
    stageData->actors[6] = koboldActor4;
    stageData->actors[7] = koboldActor5;
    stageData->actors[8] = koboldActor6;
    stageData->actors[9] = koboldActor7;
    stageData->actors[10] = SluttyGenetActor;
    stageData->actors[11] = usherKoboldActor;
    stageData->actors[12] = usherKoboldActor2;
    stageData->actors[13] = koboldActor8;
    stageData->actors[14] = koboldActor9;
}

#endif //STAGE_DATA_H
