#include "MindPlaceState.h"

#include "Camera.h"
#include "Draggable.h"
#include "GameObject.h"
#include "Interactable.h"
#include "InputManager.h"
#include "SpriteRenderer.h"
#include "Text.h"
#include "DetailContent.h"
#include "Inventory.h"
#include "InventoryCatalog.h"
#include "InventoryEntryDefinition.h"
#include "InstructionPrompt.h"
#include <GameData.h>

namespace
{
const Vec2 mindPlaceBackgroundCenter(600.0f, 450.0f);
const float mindPlaceBackgroundScale = 1.0f;
}

MindPlaceState::MindPlaceState()
        : State(),
            tabs(*this),
            detailPanel(*this,
                                    mindPlaceBackgroundCenter,
                                    mindPlaceBackgroundScale,
                                    "recursos/img/espacoMental.png")
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
    SpriteRenderer* backgroundSR = new SpriteRenderer(*backgroundGO, "recursos/img/espacoMentalBackground.png");
    backgroundSR->SetScale(900.0f / 2160.0f, 900.0f / 2160.0f);
    backgroundSR->SetUseSourceFrameOffset(false);
    backgroundGO->AddComponent(backgroundSR);
    backgroundGO->box.SetCenter(mindPlaceBackgroundCenter);
    AddObject(backgroundGO);

    GameObject* boardGO = new GameObject();
    SpriteRenderer* boardSR = new SpriteRenderer(*boardGO, "recursos/img/espacoMental.png");
    boardSR->SetScale(1.344f, 1.344f);
    boardSR->SetUseSourceFrameOffset(false);
    boardGO->AddComponent(boardSR);
    // The artwork has transparent padding; this center places its opaque board at (600, 470).
    boardGO->box.SetCenter(Vec2(589.0f, 582.0f));
    AddObject(boardGO);

    int tab0 = tabs.AddTab(Vec2(243.0f, 282.0f),
                           "recursos/img/abaNSelecionada.png",
                           "recursos/img/abaSelecionada.png");
    int tab1 = tabs.AddTab(Vec2(483.0f, 282.0f),
                           "recursos/img/abaNSelecionada.png",
                           "recursos/img/abaSelecionada.png");
    int tab2 = tabs.AddTab(Vec2(723.0f, 282.0f),
                           "recursos/img/abaNSelecionada.png",
                           "recursos/img/abaSelecionada.png");

    int counts[] = {0, 0, 0};
    for (const std::string &entryId : Inventory::GetEntries()) {
        const InventoryEntryDefinition *definition = InventoryCatalog::Find(entryId);
        if (!definition) continue;
        int tab = definition->category == MindPlaceCategory::People ? tab0
                : definition->category == MindPlaceCategory::Documents ? tab1 : tab2;
        const int categoryIndex = tab == tab0 ? 0 : tab == tab1 ? 1 : 2;
        const int index = counts[categoryIndex]++;
        Vec2 center(240.0f + (index % 4) * 220.0f, 365.0f + (index / 4) * 170.0f);
        tabs.AddContent(tab, CreateInventoryEntry(center, entryId));
    }

    tabs.SwitchTo(0);

    StartArray();
    started = true;

    started = true;

    if (!GameData::GetFlag("instrucao_mente_mostrada")) {
        GameData::SetFlag("instrucao_mente_mostrada", true);
        
        GameObject* promptGO = new GameObject();
        promptGO->AddComponent(new Text(*promptGO, "recursos/font/neodgm.ttf", 26, Text::BLENDED, 
            "Clique nas pastas para ler conversas antigas | S: Voltar", {0, 0, 0, 255}));
        promptGO->AddComponent(new InstructionPrompt(*promptGO, 8.0f, Vec2(600.0f, 150.0f)));
        AddObject(promptGO);
    }

}

std::weak_ptr<GameObject> MindPlaceState::CreateEntry(const Vec2 &center,
                                                       const std::string &iconPath,
                                                       std::function<void()> callback,
                                                       float scale)
{
    const float offscreenY = 2000.0f;
    GameObject *entry = new GameObject();
    SpriteRenderer *sprite = new SpriteRenderer(*entry, iconPath);
    sprite->SetScale(scale, scale);
    sprite->SetUseSourceFrameOffset(false);
    entry->AddComponent(sprite);
    entry->AddComponent(new Interactable(*entry, Interactable::CLICK_ONLY,
                                          Interactable::NO_ACTOR, 0.0f, callback));
    entry->box.SetCenter(Vec2(center.x, center.y + offscreenY));
    return AddObject(entry);
}

std::weak_ptr<GameObject> MindPlaceState::CreateInventoryEntry(const Vec2 &center,
                                                                const std::string &entryId)
{
    const InventoryEntryDefinition *definition = InventoryCatalog::Find(entryId);
    if (!definition) return std::weak_ptr<GameObject>();
    const std::string icon = definition->mindPlaceImage;
    return CreateEntry(center, icon, [this, entryId]() {
        const InventoryEntryDefinition *current = InventoryCatalog::Find(entryId);
        if (!current) return;
        DetailContent content{current->name, current->description, current->detailImage};
        if (current->kind == InventoryEntryKind::DialogueFolder) {
            content.description = Inventory::GetDialogueHistory(current->characterId);
        }
        detailPanel.Open(content);
    });
}

void MindPlaceState::LoadAssets()
{
}

// criar pastas padrao que so mostram seu conteudo
std::weak_ptr<GameObject> MindPlaceState::CreateFolder(const Vec2 &center,
                                                       float scale,
                                                       const std::string &spritePath)
{
    return CreateFolder(center,
                        [this]() { detailPanel.Open(); },
                        scale,
                        spritePath);
}

//interactable = nullptr para testar a funcao de arrastar, passar funcao lambda para interacoes especificas
std::weak_ptr<GameObject> MindPlaceState::CreateFolder(const Vec2 &center,
                                                       std::function<void()> interactable,
                                                       float scale,
                                                       const std::string &spritePath)
{
    const float offscreenY = 2000.0f;

    GameObject *folderGO = new GameObject();
    SpriteRenderer *sprite = new SpriteRenderer(*folderGO, spritePath);
    sprite->SetScale(scale, scale);
    sprite->SetUseSourceFrameOffset(false);
    folderGO->AddComponent(sprite);

    if (!interactable)
    {
        folderGO->AddComponent(new Draggable(*folderGO, true));
    }
    else
    {
        folderGO->AddComponent(new Interactable(
            *folderGO,
            Interactable::CLICK_ONLY,
            Interactable::NO_ACTOR,
            0.0f,
            interactable));
    }

    folderGO->box.SetCenter(Vec2(center.x, center.y + offscreenY));

    if (!interactable)
    {
        folderGO->GetComponent<Draggable>()->SetSpawnPosition(center);
    }

    return AddObject(folderGO);
}

void MindPlaceState::Update(float dt)
{
    InputManager& input = InputManager::GetInstance();

    if (input.QuitRequested())
    {
        quitRequested = true;
    }

    if (input.KeyPress(ESCAPE_KEY) || input.KeyPress(MIND_PLACE_KEY))
    {
        if (detailPanel.IsVisible())
        {
            detailPanel.Close();
        }
        else
        {
            popRequested = true;
        }
        return;
    }

    detailPanel.Update(dt);

    if (detailPanel.IsVisible() && input.GetMouseWheelY() != 0)
    {
        detailPanel.Scroll(-input.GetMouseWheelY() * 36);
    }

    if (!detailPanel.IsVisible() && input.MousePress(LEFT_MOUSE_BUTTON))
    {
        Vec2 mouseWorldPoint(input.GetMouseX() + Camera::pos.x,
                             input.GetMouseY() + Camera::pos.y);
        ActivateInteractableAtPoint(mouseWorldPoint);
    }

    if (!detailPanel.IsVisible())
    {
        UpdateArray(dt);
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
