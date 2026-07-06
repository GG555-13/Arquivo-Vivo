#include "ClueBoardState.h"

#include "Camera.h"
#include "ClueBoardData.h"
#include "Component.h"
#include "Draggable.h"
#include "Game.h"
#include "GameObject.h"
#include "InputManager.h"
#include "Interactable.h"
#include "Inventory.h"
#include "InventoryCatalog.h"
#include "InventoryEntryDefinition.h"
#include "SpriteRenderer.h"

namespace
{
const float NOTEBOOK_THUMBNAIL_SCALE = 190.0f / 891.0f;
const float NOTEBOOK_PREVIEW_SCALE = 380.0f / 891.0f;
const Vec2 PREVIEW_CENTER(984.0f, 360.0f);

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

    Vec2 initialPosition = slot.trayPosition;
    ClueBoardData::GetPlacedPosition(slot.entryId, initialPosition);
    clue->box.SetCenter(initialPosition);
    const std::string entryId = slot.entryId;
    Draggable *drag = new Draggable(*clue, false);
    drag->SetSpawnPosition(slot.trayPosition);
    drag->SetOnClick([this, entryId]() { ShowPreview(entryId); });
    drag->SetOnRelease([entryId, tray = slot.trayPosition, clue](const Vec2 &releasePoint) {
            if (!ClueBoardData::GetPlacementArea().Contains(releasePoint))
            {
                clue->box.SetCenter(tray);
                ClueBoardData::ClearPlacedPosition(entryId);
                return true;
            }

            ClueBoardData::SetPlacedPosition(entryId, releasePoint);
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
