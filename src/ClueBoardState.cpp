#include "ClueBoardState.h"

#include "Camera.h"
#include "ClueBoardData.h"
#include "Draggable.h"
#include "GameObject.h"
#include "InputManager.h"
#include "SpriteRenderer.h"
#include "Text.h"

ClueBoardState::ClueBoardState()
    : State()
{
}

ClueBoardState::~ClueBoardState()
{
}

void ClueBoardState::LoadAssets()
{
}

void ClueBoardState::Start()
{
    LoadAssets();

    Camera::pos = Vec2(0, 0);

    // Title
    GameObject* titleGO = new GameObject();
    titleGO->box.x = 320;
    titleGO->box.y = 50;
    titleGO->AddComponent(new Text(
        *titleGO,
        "recursos/font/neodgm.ttf",
        48,
        Text::BLENDED,
        "Quadro de Pistas",
        {255, 255, 255, 255}
    ));
    AddObject(titleGO);

    // Help text
    GameObject* helpGO = new GameObject();
    helpGO->box.x = 290;
    helpGO->box.y = 490;
    helpGO->AddComponent(new Text(
        *helpGO,
        "recursos/font/neodgm.ttf",
        22,
        Text::BLENDED,
        "Arraste as pistas | ESC para voltar",
        {180, 180, 180, 255}
    ));
    AddObject(helpGO);

    // Board background
    GameObject* boardGO = new GameObject();
    SpriteRenderer* boardSR = new SpriteRenderer(*boardGO, "recursos/img/quadro_pistas.png");
    boardSR->SetScale(0.9f, 0.9f);
    boardGO->AddComponent(boardSR);
    boardGO->box.SetCenter(Vec2(430.0f, 290.0f));
    AddObject(boardGO);

    // Load persisted clue positions, or use defaults on first open
    if (ClueBoardData::cluePositions.empty())
    {
        ClueBoardData::cluePositions = ClueBoardData::GetDefaultPositions();
    }

    // Create clue objects at saved positions
    for (const Vec2& pos : ClueBoardData::cluePositions)
    {
        AddClue(pos);
    }

    StartArray();
    started = true;
}

std::weak_ptr<GameObject> ClueBoardState::AddClue(const Vec2& center)
{
    GameObject* clueGO = new GameObject();
    SpriteRenderer* sprite = new SpriteRenderer(*clueGO, "recursos/img/esboçoPasta.png");
    sprite->SetScale(0.65f, 0.65f);
    clueGO->AddComponent(sprite);
    clueGO->box.SetCenter(center);

    // Draggable: do NOT return to spawn — player's position sticks
    Draggable* drag = new Draggable(*clueGO, false);
    clueGO->AddComponent(drag);

    // On release, persist the new position
    size_t clueIndex = clueObjects.size();
    drag->SetOnRelease([this, clueIndex](const Vec2& releasePoint) -> bool {
        if (clueIndex < ClueBoardData::cluePositions.size())
        {
            ClueBoardData::cluePositions[clueIndex] = releasePoint;
        }
        return true;
    });

    std::weak_ptr<GameObject> wp = AddObject(clueGO);
    clueObjects.push_back(wp);
    return wp;
}

void ClueBoardState::SaveCluePositions()
{
    for (size_t i = 0; i < clueObjects.size(); i++)
    {
        std::shared_ptr<GameObject> obj = clueObjects[i].lock();
        if (obj)
        {
            if (i < ClueBoardData::cluePositions.size())
            {
                ClueBoardData::cluePositions[i] = obj->box.Center();
            }
        }
    }
}

void ClueBoardState::Update(float dt)
{
    InputManager& input = InputManager::GetInstance();

    if (input.QuitRequested())
    {
        quitRequested = true;
    }

    if (input.KeyPress(ESCAPE_KEY))
    {
        SaveCluePositions();
        popRequested = true;
        return;
    }

    UpdateArray(dt);
}

void ClueBoardState::Render()
{
    RenderArray();
}

void ClueBoardState::Pause()
{
}

void ClueBoardState::Resume()
{
}
