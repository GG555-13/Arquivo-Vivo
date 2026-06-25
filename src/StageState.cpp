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

StageState::StageState(std::string stageId) : WalkableState(), currentStageId(stageId) {}

void StageState::Start() {
    LoadAssets();
    StageConfig config = StageManager::GetStage(currentStageId);
    LoadStage(config);
    started = true;
    StartArray();
}

void StageState::LoadBackgroundLayers(const std::vector<BackgroundLayerConfig>& layers) {
    for (const auto& layer : layers) {
        GameObject* layerGo = new GameObject();
        SpriteRenderer* layerSprite = new SpriteRenderer(*layerGo, layer.file);
        layerSprite->SetScale(layer.scaleX, layer.scaleY);
        layerSprite->SetParallax(Vec2(layer.parallaxX, layer.parallaxY));
        layerGo->AddComponent(layerSprite);
        layerGo->box.x = 0;
        layerGo->box.y = 900.0f - layerSprite->GetHeight();
        if (!layer.isSky) this->maxStageWidth = layerSprite->GetWidth(); 
        AddObject(layerGo);
    }
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

    GameObject* npcGO = new GameObject();
    
    npcGO->box.x = config.playerSpawn.x + 200.0f; 
    npcGO->box.y = config.playerSpawn.y;

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