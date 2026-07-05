#include "TitleState.h"
#include "StageState.h"
#include "InputManager.h"
#include "Game.h"
#include "SpriteRenderer.h"
#include "GameObject.h"
#include "Camera.h"

TitleState::TitleState() : State()
{
}

TitleState::~TitleState()
{
}

void TitleState::Start()
{

    GameObject* bg = new GameObject();
    SpriteRenderer* bgSprite = new SpriteRenderer(*bg, "recursos/img/Title.png");

    bg->AddComponent(bgSprite);

    bg->box.x = 0;
    bg->box.y = 0;

    AddObject(bg);

    Camera::pos = Vec2(236.0f, 20.5f);
    Camera::Unfollow();


    backgroundMusic.Open("recursos/audio/Title.mp3");
    backgroundMusic.Play(-1);

    started = true;
    StartArray();
}

void TitleState::LoadAssets()
{

}

void TitleState::Update(float dt)
{
    InputManager& input = InputManager::GetInstance();

    if (input.QuitRequested() || input.KeyPress(ESCAPE_KEY))
    {
        quitRequested = true;
    }

    if (input.KeyPress(SPACE_KEY) || input.MousePress(LEFT_MOUSE_BUTTON))
    {
        backgroundMusic.Stop(1000);

        popRequested = true;
        Game::GetInstance().Push(new StageState());
    }

    UpdateArray(dt);
}

void TitleState::Render()
{
    RenderArray();
}

void TitleState::Pause()
{
}

void TitleState::Resume()
{
    Camera::pos = Vec2(0, 0);
}