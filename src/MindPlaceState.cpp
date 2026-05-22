#include "MindPlaceState.h"

#include "Camera.h"
#include "GameObject.h"
#include "InputManager.h"
#include "Text.h"
#include "SpriteRenderer.h"

MindPlaceState::MindPlaceState() : State()
{
}

MindPlaceState::~MindPlaceState()
{
}

void MindPlaceState::Start()
{
    LoadAssets();

    Camera::pos = Vec2(0, 0);

    GameObject* titleGO = new GameObject();
    titleGO->box.x = 320;
    titleGO->box.y = 140;
    titleGO->AddComponent(new Text(
        *titleGO,
        "recursos/font/neodgm.ttf",
        48,
        Text::BLENDED,
        "Mind Place",
        {255, 255, 255, 255}
    ));
    AddObject(titleGO);

    GameObject* helpGO = new GameObject();
    helpGO->box.x = 220;
    helpGO->box.y = 260;
    helpGO->AddComponent(new Text(
        *helpGO,
        "recursos/font/neodgm.ttf",
        28,
        Text::BLENDED,
        "S ou ESC para voltar",
        {180, 180, 180, 255}
    ));
    AddObject(helpGO);

    GameObject* backgroundGO = new GameObject();
    SpriteRenderer* backgroundSR = new SpriteRenderer(*backgroundGO, "recursos/img/EsboçoMental.png");
    backgroundSR->SetScale(0.56782f, 0.56782f);
    backgroundGO->AddComponent(backgroundSR);
    backgroundGO->box.SetCenter(Vec2(180.79f, 107.5f));
    AddObject(backgroundGO);

    StartArray();
    started = true;
}

void MindPlaceState::LoadAssets()
{
}

void MindPlaceState::Update(float dt)
{
    UpdateArray(dt);

    InputManager& input = InputManager::GetInstance();

    if (input.QuitRequested())
    {
        quitRequested = true;
    }

    if (input.KeyPress(ESCAPE_KEY) || input.KeyPress(MIND_PLACE_KEY))
    {
        popRequested = true;
    }
}

void MindPlaceState::Render()
{
    RenderArray();
}

void MindPlaceState::Pause()
{
}

void MindPlaceState::Resume()
{
}