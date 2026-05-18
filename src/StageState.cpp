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

StageState::StageState() : State()
{
}

StageState::~StageState()
{
}

void StageState::Start()
{
    LoadAssets();

    // SKY
    GameObject* skyGo = new GameObject(); 
    SpriteRenderer* skySprite = new SpriteRenderer(*skyGo, "recursos/img/NightSky.png");
    skySprite->SetParallax(Vec2(0.3f, 0.3f)); 
    skyGo->AddComponent(skySprite);
    skyGo->box.x = 0;
    skyGo->box.y = 900.0f - skySprite->GetHeight(); 
    AddObject(skyGo);

    // 2. MANSION (
    GameObject* mansionGo = new GameObject();
    SpriteRenderer* mansionSprite = new SpriteRenderer(*mansionGo, "recursos/img/Mansion.jpg");
    mansionSprite->SetScale(0.7f, 0.7f);
    mansionSprite->SetParallax(Vec2(1.0f, 1.0f)); 
    mansionGo->AddComponent(mansionSprite);
    mansionGo->box.x = 0;
    mansionGo->box.y = 900.0f - mansionSprite->GetHeight(); 
    AddObject(mansionGo);

    // 3. PLAYER
    GameObject *playerGo = new GameObject();
    playerGo->box.x = 200;
    playerGo->box.y = 700; 
    playerGo->AddComponent(new Character(*playerGo, "recursos/img/Player.png"));
    playerGo->AddComponent(new PlayerController(*playerGo));
    AddObject(playerGo);

    Camera::Follow(playerGo);

    started = true;
    StartArray();
}

void StageState::LoadAssets()
{
    music.Open("recursos/audio/BGM.wav");
    music.Play(-1);
}

void StageState::Update(float dt)
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