#include "TutorialEndInterludeState.h"

#include "Camera.h"
#include "ClueBoardData.h"
#include "FadeTypes.h"
#include "Game.h"
#include "GameObject.h"
#include "Inventory.h"
#include "SpriteRenderer.h"

#include <algorithm>

namespace
{
const float HOLD_DURATION = 3.0f;
const float FADE_DURATION = 1.0f;
}

TutorialEndInterludeState::TutorialEndInterludeState()
    : State()
{
}

TutorialEndInterludeState::~TutorialEndInterludeState()
{
}

void TutorialEndInterludeState::LoadAssets()
{
}

void TutorialEndInterludeState::Start()
{
    LoadAssets();
    Camera::pos = Vec2(0.0f, 0.0f);

    GameObject *background = new GameObject();
    SpriteRenderer *sprite = new SpriteRenderer(*background, "recursos/img/FUNDO_DO_JOGO.png");
    const float scaleX = static_cast<float>(Game::GetInstance().GetWidth()) / static_cast<float>(sprite->GetWidth());
    const float scaleY = static_cast<float>(Game::GetInstance().GetHeight()) / static_cast<float>(sprite->GetHeight());
    const float scale = std::max(scaleX, scaleY);
    sprite->SetScale(scale, scale);
    sprite->SetUseSourceFrameOffset(false);
    background->AddComponent(sprite);
    background->box.SetCenter(Vec2(Game::GetInstance().GetWidth() / 2.0f,
                                   Game::GetInstance().GetHeight() / 2.0f));
    AddObject(background);

    started = true;
    StartArray();
    screenFade.FadeIn(FADE_DURATION, FadeColor::Black);
}

void TutorialEndInterludeState::Update(float dt)
{
    screenFade.Update(dt);
    if (screenFade.IsActive())
    {
        return;
    }

    if (!fadeOutStarted)
    {
        holdTimer.Update(dt);
        if (holdTimer.Get() >= HOLD_DURATION)
        {
            fadeOutStarted = true;
            screenFade.FadeOut(FADE_DURATION, FadeColor::Black, [this]() {
                CleanupTutorialData();
                Game::GetInstance().SetPendingFadeIn({true, FADE_DURATION, FadeColor::Black});
                popRequested = true;
            });
        }
    }

    UpdateArray(dt);
}

void TutorialEndInterludeState::Render()
{
    RenderArray();
    screenFade.Render();
}

void TutorialEndInterludeState::Pause()
{
}

void TutorialEndInterludeState::Resume()
{
}

void TutorialEndInterludeState::CleanupTutorialData()
{
    ClueBoardData::ClearProgress();
    Inventory::Remove("tutorial_notebook_1");
    Inventory::Remove("tutorial_notebook_2");
    Inventory::Remove("cafe");
    Inventory::Remove("bread_de_queijo");
    Inventory::Remove("relato_1");
    Inventory::Remove("relato_2");
    Inventory::Remove("relato_3");
    Inventory::Remove("chief_dialogues");
}
