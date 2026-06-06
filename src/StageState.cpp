#include "StageState.h"
#include "SpriteRenderer.h"
#include "Game.h"
#include "TileSet.h"
#include "TileMap.h"
#include "InputManager.h"
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


StageState::StageState(std::string stageId) : walkableState(), currentStageId(stageId) {
}

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
}

void StageState::TransitionTo(std::string targetStageId) {
    popRequested = true;
    Game::GetInstance().Push(new StageState(targetStageId));
}

void StageState::LoadAssets()
{
    music.Open("recursos/audio/BGM.wav");
    music.Play(-1);
}

void StageState::UpdateWalkable(float dt)
{
    if (InputManager::GetInstance().QuitRequested())
    {
        quitRequested = true;
    }
    if (InputManager::GetInstance().KeyPress(ESCAPE_KEY))
    {
        popRequested = true;
    }

    Camera::Update(dt);
    UpdateArray(dt);

    for (unsigned i = 0; i < objectArray.size(); i++) 
    {
        Collider* colA = objectArray[i]->GetComponent<Collider>();
        if (!colA) continue;

        for (unsigned j = i + 1; j < objectArray.size(); j++) 
        {
            Collider* colB = objectArray[j]->GetComponent<Collider>();
            if (!colB) continue; 

            if (Collision::IsColliding(colA->box, colB->box, objectArray[i]->angleDeg, objectArray[j]->angleDeg)) 
            {
                objectArray[i]->NotifyCollision(*objectArray[j]);
                objectArray[j]->NotifyCollision(*objectArray[i]);
            }
        }
    }
}

void StageState::Render()
{

    RenderArray();

}

void StageState::Pause()
{
}

void StageState::Resume()
{
}


StageState::~StageState() {

}

void StageState::Update(float dt) {

    UpdateWalkable(dt);
}