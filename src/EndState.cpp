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
    bg->box.x = 0;
    bg->box.y = 0;

    if (GameData::playerVictory)
    {
        bg->AddComponent(new SpriteRenderer(*bg, "recursos/img/Win.png"));
        backgroundMusic.Open("recursos/audio/endStateWin.ogg");
    }
    else
    {
        bg->AddComponent(new SpriteRenderer(*bg, "recursos/img/Lose.png"));
        backgroundMusic.Open("recursos/audio/endStateLose.ogg");
    }
    AddObject(bg);

    backgroundMusic.Play(-1);

    GameObject *textGO = new GameObject();
    textGO->box.x = 300;
    textGO->box.y = 500;
    SDL_Color color = {255, 0, 0, 255};

    Text *text = new Text(*textGO, "recursos/font/neodgm.ttf", 40, Text::SOLID, "ESC para Sair - ESPACO para Menu", color);
    textGO->AddComponent(text);
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