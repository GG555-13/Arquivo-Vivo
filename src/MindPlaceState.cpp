#include "MindPlaceState.h"

#include "Camera.h"
#include "Draggable.h"
#include "GameObject.h"
#include "InputManager.h"
#include "SpriteRenderer.h"
#include "Text.h"

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
    SpriteRenderer* backgroundSR = new SpriteRenderer(*backgroundGO, "recursos/img/EsboçoMental2.png");
    backgroundSR->SetScale(0.56782f, 0.56782f);
    backgroundGO->AddComponent(backgroundSR);
    backgroundGO->box.SetCenter(Vec2(180.79f, 107.5f));
    AddObject(backgroundGO);

    GameObject* folderGO = new GameObject();
    SpriteRenderer* folderSR = new SpriteRenderer(*folderGO, "recursos/img/esboçoPasta.png");
    folderSR->SetScale(0.65f, 0.65f);
    folderGO->AddComponent(folderSR);
    folderGO->AddComponent(new Draggable(*folderGO, true));
    folderGO->box.SetCenter(Vec2(190.0f, 210.0f));
    folderGO->GetComponent<Draggable>()->SetSpawnPosition(folderGO->box.Center());
    AddObject(folderGO);

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