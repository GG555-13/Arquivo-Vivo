#include "TitleState.h"
#include "StageState.h"
#include "InputManager.h"
#include "Game.h"
#include "SpriteRenderer.h"
#include "GameObject.h"
#include "Camera.h"

#include <algorithm>

TitleState::TitleState() : State()
{
}

TitleState::~TitleState()
{
}

void TitleState::Start()
{

    GameObject* bg = new GameObject();
    SpriteRenderer* bgSprite = new SpriteRenderer(*bg, "recursos/img/FUNDO_DO_JOGO.png");
    const float scaleX = static_cast<float>(Game::GetInstance().GetWidth()) / static_cast<float>(bgSprite->GetWidth());
    const float scaleY = static_cast<float>(Game::GetInstance().GetHeight()) / static_cast<float>(bgSprite->GetHeight());
    const float scale = std::max(scaleX, scaleY);
    bgSprite->SetScale(scale, scale);
    bgSprite->SetUseSourceFrameOffset(false);

    bg->AddComponent(bgSprite);

    bg->box.SetCenter(Vec2(Game::GetInstance().GetWidth() / 2.0f,
                           Game::GetInstance().GetHeight() / 2.0f));

    AddObject(bg);

    Camera::pos = Vec2(0.0f, 0.0f);
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

    screenFade.Update(dt);
    if (startingGame)
    {
        UpdateArray(dt);
        return;
    }

    if (input.QuitRequested() || input.KeyPress(ESCAPE_KEY))
    {
        quitRequested = true;
    }

    if (input.KeyPress(SPACE_KEY) || input.MousePress(LEFT_MOUSE_BUTTON))
    {
        startingGame = true;
        backgroundMusic.Stop(1000);
        screenFade.FadeOut(1.0f, FadeColor::Black, [this]() {
            Game::GetInstance().SetPendingFadeIn({true, 1.0f, FadeColor::Black});
            popRequested = true;
            Game::GetInstance().Push(new StageState("mansion_interior", 950.0f, 760.0f));
        });
    }

    UpdateArray(dt);
}

void TitleState::Render()
{
    RenderArray();
    screenFade.Render();
}

void TitleState::Pause()
{
}

void TitleState::Resume()
{
    Camera::pos = Vec2(0, 0);
}
