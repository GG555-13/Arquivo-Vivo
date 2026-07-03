#include "StageState.h"
#include "SpriteRenderer.h"
#include "Game.h"
#include "TileSet.h"
#include "TileMap.h"
#include "Camera.h"
#include "Character.h"
#include "PlayerController.h"
#include "Collision.h"
#include "Collider.h"
#include <algorithm>
#include <cmath>
#include "GameData.h"
#include "EndState.h"
#include "StageManager.h"
#include "TransitionTrigger.h"
#include "Resources.h"
#include "InputManager.h"
#include "Interactable.h"
#include "DialogueBox.h"
#include "ClueBoardState.h"
#include "Interactable.h"

StageState::StageState(std::string stageId, float spawnX, float spawnY) : WalkableState(), 
                                                                          currentStageId(stageId), 
                                                                          overrideSpawnX(spawnX), 
                                                                          overrideSpawnY(spawnY) {}

void StageState::Start() {
    LoadAssets();
    StageConfig config = StageManager::GetStage(currentStageId);
    LoadStage(config);
    started = true;
    StartArray();

    auto pf = Game::GetInstance().ConsumePendingFadeIn();
    if (pf.active) {
        screenFade.FadeIn(pf.duration, pf.color);
    }
}

void StageState::LoadBackgroundLayers(const std::vector<BackgroundLayerConfig>& layers) {
    this->maxStageWidth = 0.0f;
    for (const auto& layer : layers) {
        GameObject* layerGo = new GameObject();
        SpriteRenderer* layerSprite = new SpriteRenderer(*layerGo, layer.file);
        layerSprite->SetScale(layer.scaleX, layer.scaleY);
        layerSprite->SetParallax(Vec2(layer.parallaxX, layer.parallaxY));
        layerGo->AddComponent(layerSprite);
        // Compensa offsets
        float spriteWidth = layerSprite->GetWidth();
        float spriteHeight = layerSprite->GetHeight();
        float centeringOffsetX = (spriteWidth / layer.scaleX) * (1.0f - layer.scaleX) / 2.0f;
        float centeringOffsetY = (spriteHeight / layer.scaleY) * (1.0f - layer.scaleY) / 2.0f;
        layerGo->box.x = layer.baseWidth * layer.scaleX - centeringOffsetX;
        layerGo->box.y = 900.0f - spriteHeight - centeringOffsetY;
        if (!layer.isSky) {
            float layerRightEdge = layer.baseWidth * layer.scaleX + spriteWidth;
            if (layerRightEdge > this->maxStageWidth)
                this->maxStageWidth = layerRightEdge;
        }
        AddObject(layerGo);
    }
    Camera::stageWidth = this->maxStageWidth;
}

void StageState::LoadStage(const StageConfig& config) {
    Resources::ClearImages();
    Resources::ClearMusics();
    LoadBackgroundLayers(config.layers);

    if (!config.musicFile.empty()) {
        backgroundMusic.Open(config.musicFile);
        backgroundMusic.Play(-1);
    }

    GameObject* playerGo = new GameObject();
    playerGo->box.x = (overrideSpawnX >= 0.0f) ? overrideSpawnX : config.playerSpawn.x;
    playerGo->box.y = (overrideSpawnY >= 0.0f) ? overrideSpawnY : config.playerSpawn.y;
    playerGo->AddComponent(new Character(*playerGo, "recursos/img/Protagonista.png"));
    playerGo->AddComponent(new PlayerController(*playerGo));

    float sx = (overrideSpawnX >= 0.0f) ? overrideSpawnX : config.playerSpawn.x;
    float sy = (overrideSpawnY >= 0.0f) ? overrideSpawnY : config.playerSpawn.y;
    playerGo->box.SetCenter(Vec2(sx, sy));

    AddObject(playerGo);
    Camera::Follow(playerGo);

    // Debug: player position display
    debugPosText = new GameObject();
    Text* posText = new Text(*debugPosText,
        "recursos/font/neodgm.ttf", 16, Text::SOLID,
        "Pos: (0, 0)",
        {255, 255, 0, 255});  // yellow
    debugPosText->AddComponent(posText);
    AddObject(debugPosText);

    for (const auto& triggerData : config.triggers) {
        GameObject* triggerGo = new GameObject();
        triggerGo->box.x = triggerData.x;
        triggerGo->box.y = triggerData.y;
        triggerGo->box.w = triggerData.width;
        triggerGo->box.h = triggerData.height;
        // triggerGo->AddComponent(new Collider(*triggerGo));
        // triggerGo->AddComponent(new TransitionTrigger(*triggerGo, triggerData.targetStageId));

        // triggerGo->AddComponent(new Interactable(*triggerGo,
        //                                          Interactable::SPACE_OR_CLICK,
        //                                          Interactable::REQUIRE_INSIDE_AREA,
        //                                          0.0f,
        //                                          [targetId = triggerData.targetStageId]() {
        //                                             auto* stage = static_cast<StageState*>(&Game::GetInstance().GetCurrentState());
        //                                             if (stage) stage->TransitionTo(targetId);
        //                                          },
        //                                          -28.0f
        // ));

        triggerGo->AddComponent(new Interactable(*triggerGo,
                                                 Interactable::SPACE_OR_CLICK,
                                                 Interactable::REQUIRE_NEAR,
                                                 std::max(triggerData.width, triggerData.height) * 0.7f,
                                                 [targetId = triggerData.targetStageId,
                                                  sx = triggerData.targetSpawnX,
                                                  sy = triggerData.targetSpawnY,
                                                  fadeCfg = triggerData.fade]() {
                                                     auto* stage = static_cast<StageState*>(&Game::GetInstance().GetCurrentState());
                                                     if (stage) stage->TransitionTo(targetId, sx, sy, fadeCfg);
                                                 }
        ));

        AddObject(triggerGo);
    }

    // Clue board interactable — only inside the mansion
    if (config.stageId == "mansion_interior") {
        GameObject* clueBoardGo = new GameObject();
        // SpriteRenderer* boardSprite = new SpriteRenderer(*clueBoardGo, "recursos/img/quadro_pistas.png");
        // boardSprite->SetScale(1.2f, 1.2f);
        // clueBoardGo->AddComponent(boardSprite);
        clueBoardGo->box.SetCenter(Vec2(2400.0f, 750.0f));

        clueBoardGo->AddComponent(new Interactable(
            *clueBoardGo,
            Interactable::SPACE_OR_CLICK,
            Interactable::REQUIRE_NEAR,
            250.0f,
            []() {
                Game::GetInstance().Push(new ClueBoardState());
            },
            -110.0f
        ));
        AddObject(clueBoardGo);
    }

    GameObject* npcGO = new GameObject();
    
    npcGO->box.x = config.playerSpawn.x + 200.0f; 
    npcGO->box.y = 750.0f;

    npcGO->AddComponent(new SpriteRenderer(*npcGO, "recursos/img/NPC.png", 3, 4));

    npcGO->AddComponent(new Collider(*npcGO));

    npcGO->AddComponent(new Interactable(*npcGO, Interactable::SPACE_ONLY, Interactable::REQUIRE_NEAR, 100.0f, []() {
        if (DialogueBox::isPlaying) return; 
        GameObject* dialogueController = new GameObject();
        dialogueController->AddComponent(new DialogueBox(*dialogueController, "recursos/dialogos/clue01.json"));
        Game::GetInstance().GetCurrentState().AddObject(dialogueController);
    }));

    AddObject(npcGO);

}


void StageState::PerformTransitionTo(std::string targetStageId, float spawnX, float spawnY) {
    popRequested = true;

    if (targetStageId == "WIN_GAME") {
        GameData::playerVictory = true;
        Game::GetInstance().Push(new EndState());
    }
    else if (targetStageId == "LOSE_GAME") {
        GameData::playerVictory = false;
        Game::GetInstance().Push(new EndState());
    }
    else {
        Game::GetInstance().Push(new StageState(targetStageId, spawnX, spawnY));
    }
}

void StageState::TransitionTo(std::string targetStageId, float spawnX, float spawnY,
                               const FadeTransitionConfig& fadeConfig) {
    if (screenFade.IsActive()) return;  // block double-trigger while fading

    if (fadeConfig.fadeIn) {
        Game::GetInstance().SetPendingFadeIn({true, fadeConfig.fadeInDuration, fadeConfig.fadeInColor});
    }

    if (fadeConfig.fadeOut) {
        screenFade.FadeOut(fadeConfig.fadeOutDuration, fadeConfig.fadeOutColor, [=]() {
            PerformTransitionTo(targetStageId, spawnX, spawnY);
        });
        // do NOT pop yet — callback handles it when fade completes
    } else {
        PerformTransitionTo(targetStageId, spawnX, spawnY);
    }
}

void StageState::LoadAssets() {

}

void StageState::UpdateWalkable(float dt) {
    (void)dt;

    for (unsigned i = 0; i < objectArray.size(); i++) {
        Collider* colA = objectArray[i]->GetComponent<Collider>();
        if (!colA) continue;

        for (unsigned j = i + 1; j < objectArray.size(); j++) {
            Collider* colB = objectArray[j]->GetComponent<Collider>();
            if (!colB) continue; 

            if (Collision::IsColliding(colA->box, colB->box, objectArray[i]->angleDeg, objectArray[j]->angleDeg)) {
                objectArray[i]->NotifyCollision(*objectArray[j]);
                objectArray[j]->NotifyCollision(*objectArray[i]);
            }
        }
    }
}

void StageState::Render() {
    RenderArray();
    screenFade.Render();
}

void StageState::Pause() {}
void StageState::Resume() {}
StageState::~StageState() {}

void StageState::Update(float dt) {
    screenFade.Update(dt);

    if (screenFade.IsActive()) {
        return;  // block game logic + input during fade
    }

    WalkableState::Update(dt);

    // Update debug position display
    if (debugPosText && Character::player) {
        Vec2 pos = Character::player->GetPosition();
        auto* text = debugPosText->GetComponent<Text>();
        if (text) {
            text->SetText(
                "Pos: (" + std::to_string((int)pos.x) + ", " + std::to_string((int)pos.y) + ")"
            );
        }
        // Pin to top-left of screen
        debugPosText->box.x = Camera::pos.x + 10.0f;
        debugPosText->box.y = Camera::pos.y + 10.0f;
    }
}