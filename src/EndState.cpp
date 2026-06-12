#include "EndState.h"
#include "Game.h"
#include "InputManager.h"
#include "GameData.h"
#include "GameObject.h"
#include "SpriteRenderer.h"
#include "Text.h"
#include "Camera.h"

EndState::EndState() : State()
{
}

EndState::~EndState()
{
}

void EndState::Start()
{
    LoadAssets();

    Camera::pos = Vec2(0, 0);

    GameObject* bgGo = new GameObject();
    
    std::string bgFile = GameData::playerVictory ? "recursos/img/End.png" : "recursos/img/Lose.png";
    SpriteRenderer* bgSprite = new SpriteRenderer(*bgGo, bgFile);
    bgGo->AddComponent(bgSprite);
    
    float screenWidth = 1024.0f;
    float screenHeight = 600.0f;
    
    bgGo->box.x = (screenWidth - bgSprite->GetWidth()) / 2.0f;
    bgGo->box.y = (screenHeight - bgSprite->GetHeight()) / 2.0f;
    
    AddObject(bgGo);


    backgroundMusic.Play(-1);

    GameObject *textGO = new GameObject();
    SDL_Color color = {255, 0, 0, 255};

    Text *text = new Text(*textGO, "recursos/font/neodgm.ttf", 40, Text::SOLID, "ESC para Sair - ESPACO para Menu", color);
    textGO->AddComponent(text);
    
    textGO->box.SetCenter(Vec2(1200.0f / 2.0f, 800.0f));
    
    AddObject(textGO);

    StartArray();
    started = true;
}

void EndState::LoadAssets()
{
}

void EndState::Update(float dt)
{
    UpdateArray(dt);

    if (InputManager::GetInstance().QuitRequested() || InputManager::GetInstance().KeyPress(ESCAPE_KEY))
    {
        quitRequested = true;
    }

    if (InputManager::GetInstance().KeyPress(SPACE_KEY))
    {
        popRequested = true;
    }
}

void EndState::Render()
{
    RenderArray();
}

void EndState::Pause() {}
void EndState::Resume() {}