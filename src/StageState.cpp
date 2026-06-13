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
#include "ClueBoardState.h"
#include "Interactable.h"

StageState::StageState(std::string stageId) : WalkableState(), currentStageId(stageId) {}

void StageState::Start() {
    LoadAssets();
    StageConfig config = StageManager::GetStage(currentStageId);
    LoadStage(config);
    started = true;
    StartArray();
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
    playerGo->box.x = config.playerSpawn.x;
    playerGo->box.y = config.playerSpawn.y;
    playerGo->AddComponent(new Character(*playerGo, "recursos/img/Player.png"));
    playerGo->AddComponent(new PlayerController(*playerGo));
    AddObject(playerGo);
    Camera::Follow(playerGo);

    for (const auto& triggerData : config.triggers) {
        GameObject* triggerGo = new GameObject();
        triggerGo->box.x = triggerData.x;
        triggerGo->box.y = triggerData.y;
        triggerGo->box.w = triggerData.width;
        triggerGo->box.h = triggerData.height;
        triggerGo->AddComponent(new Collider(*triggerGo));
        triggerGo->AddComponent(new TransitionTrigger(*triggerGo, triggerData.targetStageId));
        AddObject(triggerGo);
    }

    // Clue board interactable — only inside the mansion
    if (config.stageId == "mansion_interior") {
        GameObject* clueBoardGo = new GameObject();
        SpriteRenderer* boardSprite = new SpriteRenderer(*clueBoardGo, "recursos/img/quadro_pistas.png");
        boardSprite->SetScale(1.2f, 1.2f);
        clueBoardGo->AddComponent(boardSprite);
        clueBoardGo->box.SetCenter(Vec2(450.0f, 650.0f));

        clueBoardGo->AddComponent(new Interactable(
            *clueBoardGo,
            Interactable::SPACE_OR_CLICK,
            Interactable::REQUIRE_NEAR,
            140.0f,
            []() {
                Game::GetInstance().Push(new ClueBoardState());
            }
        ));
        AddObject(clueBoardGo);
    }
}

void StageState::TransitionTo(std::string targetStageId) {
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
        Game::GetInstance().Push(new StageState(targetStageId));
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
}

void StageState::Pause() {}
void StageState::Resume() {}
StageState::~StageState() {}

void StageState::Update(float dt) {
    WalkableState::Update(dt);
}