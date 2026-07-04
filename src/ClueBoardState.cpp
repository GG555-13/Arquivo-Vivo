#include "ClueBoardState.h"

#include "Camera.h"
#include "ClueBoardData.h"
#include "Draggable.h"
#include "GameObject.h"
#include "InputManager.h"
#include "SpriteRenderer.h"
#include "Text.h"
#include "Inventory.h"
#include "InventoryCatalog.h"
#include "InventoryEntryDefinition.h"
#include "GameData.h"

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
    SpriteRenderer* boardSR = new SpriteRenderer(*boardGO, "recursos/img/estrutura_quadro.png");
    boardSR->SetScale(1.4f, 1.4f);
    boardGO->AddComponent(boardSR);
    boardGO->box.SetCenter(Vec2(775.0f, 610.0f));
    AddObject(boardGO);

    GameObject *progress = new GameObject();
    progress->box.x = 760.0f;
    progress->box.y = 500.0f;
    progress->AddComponent(new Text(*progress, "recursos/font/neodgm.ttf", 20, Text::BLENDED, "", {210, 210, 210, 255}));
    progressTextObject = AddObject(progress);

    size_t visibleIndex = 0;
    for (const ClueBoardSlot &slot : ClueBoardData::GetSlots()) {
        const InventoryEntryDefinition *definition = InventoryCatalog::Find(slot.entryId);
        if (!definition || !definition->usableOnClueBoard || !Inventory::Has(slot.entryId)) continue;
        Vec2 initial(260.0f + static_cast<float>(visibleIndex % 3) * 150.0f,
                     200.0f + static_cast<float>(visibleIndex / 3) * 140.0f);
        const auto saved = ClueBoardData::entryPositions.find(slot.entryId);
        if (saved != ClueBoardData::entryPositions.end()) initial = saved->second;
        if (ClueBoardData::IsLocked(slot.entryId)) initial = slot.targetPosition;
        AddClue(slot.entryId, initial);
        ++visibleIndex;
    }

    UpdateProgressText();

    StartArray();
    started = true;
}

std::weak_ptr<GameObject> ClueBoardState::AddClue(const std::string &entryId, const Vec2 &center)
{
    const InventoryEntryDefinition *definition = InventoryCatalog::Find(entryId);
    if (!definition) return std::weak_ptr<GameObject>();
    GameObject *clueGO = new GameObject();
    SpriteRenderer *sprite = new SpriteRenderer(*clueGO, definition->mindPlaceImage);
    const float width = sprite->GetWidth();
    const float scale = width > 0.0f ? 180.0f / width : 0.4f;
    sprite->SetScale(scale, scale);
    sprite->SetUseSourceFrameOffset(false);
    clueGO->AddComponent(sprite);
    clueGO->box.SetCenter(center);

    if (!ClueBoardData::IsLocked(entryId)) {
        Draggable *drag = new Draggable(*clueGO, false);
        drag->SetOnRelease([this, entryId](const Vec2 &releasePoint) {
            for (const ClueBoardSlot &slot : ClueBoardData::GetSlots()) {
                if (slot.entryId != entryId) continue;
                if (releasePoint.Distance(slot.targetPosition) <= slot.snapRadius) {
                    for (auto &item : entryClueObjects) {
                        if (item.first == entryId) {
                            if (auto object = item.second.lock()) {
                                object->box.SetCenter(slot.targetPosition);
                                if (auto draggable = object->GetComponent<Draggable>()) draggable->SetEnabled(false);
                            }
                            break;
                        }
                    }
                    ClueBoardData::entryPositions[entryId] = slot.targetPosition;
                    ClueBoardData::Lock(entryId);
                    UpdateProgressText();
                    return true;
                }
            }
            ClueBoardData::entryPositions[entryId] = releasePoint;
            return true;
        });
        clueGO->AddComponent(drag);
    }
    std::weak_ptr<GameObject> result = AddObject(clueGO);
    clueObjects.push_back(result);
    entryClueObjects.push_back(std::make_pair(entryId, result));
    return result;
}

std::weak_ptr<GameObject> ClueBoardState::AddClue(const Vec2& center)
{
    GameObject* clueGO = new GameObject();
    SpriteRenderer* sprite = new SpriteRenderer(*clueGO, "recursos/img/papel_quadro.png");
    sprite->SetScale(0.40f, 0.40f);
    sprite->SetUseSourceFrameOffset(false);
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
    for (const auto &item : entryClueObjects) {
        if (auto object = item.second.lock()) ClueBoardData::entryPositions[item.first] = object->box.Center();
    }
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

void ClueBoardState::UpdateProgressText()
{
    int total = 0;
    int solved = 0;
    for (const ClueBoardSlot &slot : ClueBoardData::GetSlots()) {
        const InventoryEntryDefinition *definition = InventoryCatalog::Find(slot.entryId);
        if (!definition || !definition->usableOnClueBoard || !Inventory::Has(slot.entryId)) continue;
        ++total;
        if (ClueBoardData::IsLocked(slot.entryId)) ++solved;
    }
    const int missing = total - solved;
    if (auto object = progressTextObject.lock()) {
        object->GetComponent<Text>()->SetText(missing == 0 && total > 0
            ? "Quadro concluído" : "Pistas restantes: " + std::to_string(missing));
    }
    if (missing == 0 && total > 0) GameData::AdvanceTutorial(TutorialStep::OpenBoard, TutorialStep::SolveBoard);
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
