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

    GameObject *bg = new GameObject();
    SpriteRenderer *bgSprite = nullptr;

    if (GameData::playerVictory)
    {
        bgSprite = new SpriteRenderer(*bg, "recursos/img/End.png");
        backgroundMusic.Open("recursos/audio/Win.mp3");
    }
    else
    {
        bgSprite = new SpriteRenderer(*bg, "recursos/img/Lose.png");
        backgroundMusic.Open("recursos/audio/GameOver.mp3");
    }

    float scale = 1200.0f / bgSprite->GetWidth(); 
    bgSprite->SetScale(scale, scale); 
    bg->AddComponent(bgSprite);

    bg->box.SetCenter(Vec2(1200.0f / 2.0f, 900.0f / 2.0f));

    AddObject(bg);

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