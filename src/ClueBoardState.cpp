#include "ClueBoardState.h"

#include "Camera.h"
#include "ClueBoardData.h"
#include "Component.h"
#include "Draggable.h"
#include "Game.h"
#include "GameData.h"
#include "GameObject.h"
#include "InputManager.h"
#include "Interactable.h"
#include "Inventory.h"
#include "InventoryCatalog.h"
#include "InventoryEntryDefinition.h"
#include "SpriteRenderer.h"

#include <algorithm>

namespace
{
const float NOTEBOOK_THUMBNAIL_SCALE = 190.0f / 891.0f;
const float NOTEBOOK_PREVIEW_SCALE = 380.0f / 891.0f;
const Vec2 PREVIEW_CENTER(984.0f, 360.0f);

class SlotRenderer : public Component
{
public:
    explicit SlotRenderer(GameObject &associated)
        : Component(associated)
    {
    }

    void Update(float) override {}

    void Render() override
    {
        SDL_Renderer *renderer = Game::GetInstance().GetRenderer();
        SDL_SetRenderDrawBlendMode(renderer, SDL_BLENDMODE_BLEND);
        SDL_SetRenderDrawColor(renderer, 145, 145, 145, 150);
        SDL_Rect rect = {
            static_cast<int>(associated.box.x - Camera::pos.x),
            static_cast<int>(associated.box.y - Camera::pos.y),
            static_cast<int>(associated.box.w),
            static_cast<int>(associated.box.h)
        };
        SDL_RenderFillRect(renderer, &rect);
    }
};
}

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

    GameObject *board = new GameObject();
    SpriteRenderer *boardSprite = new SpriteRenderer(*board, "recursos/img/estrutura_quadro.png");
    boardSprite->SetScale(1200.0f / 831.0f, 900.0f / 535.0f);
    boardSprite->SetUseSourceFrameOffset(false);
    board->AddComponent(boardSprite);
    board->box.SetCenter(Vec2(600.0f, 450.0f));
    AddObject(board);

    AddQuestionPaper();

    for (const ClueBoardSlot &slot : ClueBoardData::GetSlots())
    {
        AddPuzzleSlot(slot.targetPosition);
    }

    GameObject *preview = new GameObject();
    preview->box.SetCenter(PREVIEW_CENTER);
    previewObject = AddObject(preview);

    for (const ClueBoardSlot &slot : ClueBoardData::GetSlots())
    {
        if (Inventory::Has(slot.entryId))
        {
            AddClue(slot);
        }
    }

    UpdateCompletion();
    StartArray();
    started = true;
}

void ClueBoardState::AddQuestionPaper()
{
    GameObject *paper = new GameObject();
    SpriteRenderer *sprite = new SpriteRenderer(*paper, "recursos/img/papel_quadro.png");
    sprite->SetScale(0.75f, 0.75f);
    sprite->SetUseSourceFrameOffset(false);
    paper->AddComponent(sprite);

    // Compensates for the paper occupying the left side of a transparent canvas.
    paper->box.SetCenter(Vec2(348.0f, 227.0f));
    AddObject(paper);
}

void ClueBoardState::AddPuzzleSlot(const Vec2 &center)
{
    GameObject *slot = new GameObject();
    slot->box.w = 190.0f;
    slot->box.h = 140.0f;
    slot->box.SetCenter(center);
    slot->AddComponent(new SlotRenderer(*slot));
    AddObject(slot);
}

void ClueBoardState::AddClue(const ClueBoardSlot &slot)
{
    const InventoryEntryDefinition *definition = InventoryCatalog::Find(slot.entryId);
    if (!definition || !definition->usableOnClueBoard)
    {
        return;
    }

    GameObject *clue = new GameObject();
    SpriteRenderer *sprite = new SpriteRenderer(*clue, definition->mindPlaceImage);
    sprite->SetScale(NOTEBOOK_THUMBNAIL_SCALE, NOTEBOOK_THUMBNAIL_SCALE);
    sprite->SetUseSourceFrameOffset(false);
    clue->AddComponent(sprite);

    const bool locked = ClueBoardData::IsLocked(slot.entryId);
    const Vec2 initialPosition = locked ? slot.targetPosition : slot.trayPosition;
    clue->box.SetCenter(initialPosition);
    const std::string entryId = slot.entryId;
    Draggable *drag = new Draggable(*clue, true);
    drag->SetSpawnPosition(initialPosition);
    drag->SetOnClick([this, entryId]() { ShowPreview(entryId); });
    drag->SetDragEnabled(!locked);
    drag->SetOnRelease([this, entryId, target = slot.targetPosition, radius = slot.snapRadius, clue, drag](const Vec2 &releasePoint) {
            if (releasePoint.Distance(target) > radius)
            {
                return false;
            }

            clue->box.SetCenter(target);
            drag->SetSpawnPosition(target);
            drag->SetDragEnabled(false);
            ClueBoardData::Lock(entryId);
            UpdateCompletion();
            return true;
    });
    clue->AddComponent(drag);

    AddObject(clue);
}

void ClueBoardState::ShowPreview(const std::string &entryId)
{
    const InventoryEntryDefinition *definition = InventoryCatalog::Find(entryId);
    std::shared_ptr<GameObject> preview = previewObject.lock();
    if (!definition || !preview)
    {
        return;
    }

    if (SpriteRenderer *oldSprite = preview->GetComponent<SpriteRenderer>())
    {
        preview->RemoveComponent(oldSprite);
    }

    SpriteRenderer *sprite = new SpriteRenderer(*preview, definition->mindPlaceImage);
    sprite->SetScale(NOTEBOOK_PREVIEW_SCALE, NOTEBOOK_PREVIEW_SCALE);
    sprite->SetUseSourceFrameOffset(false);
    preview->AddComponent(sprite);
    preview->box.SetCenter(PREVIEW_CENTER);
}

void ClueBoardState::UpdateCompletion()
{
    const std::vector<ClueBoardSlot> &slots = ClueBoardData::GetSlots();
    if (slots.empty())
    {
        return;
    }

    const bool complete = std::all_of(slots.begin(), slots.end(), [](const ClueBoardSlot &slot) {
        return ClueBoardData::IsLocked(slot.entryId);
    });

    if (complete)
    {
        GameData::AdvanceTutorial(TutorialStep::OpenBoard, TutorialStep::SolveBoard);
    }
}

void ClueBoardState::Update(float dt)
{
    InputManager &input = InputManager::GetInstance();

    if (input.QuitRequested())
    {
        quitRequested = true;
    }

    if (input.KeyPress(ESCAPE_KEY))
    {
        popRequested = true;
        return;
    }

    if (input.MousePress(LEFT_MOUSE_BUTTON))
    {
        ActivateInteractableAtPoint(Vec2(input.GetMouseX() + Camera::pos.x,
                                         input.GetMouseY() + Camera::pos.y));
    }

    UpdateArray(dt);
}

void ClueBoardState::Render()
{
    RenderArray();
}

void ClueBoardState::Pause() {}
void ClueBoardState::Resume() {}
