#include "MindPlaceState.h"

#include "Camera.h"
#include "Draggable.h"
#include "GameObject.h"
#include "Interactable.h"
#include "InputManager.h"
#include "SpriteRenderer.h"
#include "Text.h"

namespace
{
const Vec2 mindPlaceBackgroundCenter(180.79f, 107.5f);
const float mindPlaceBackgroundScale = 0.56782f;
}

MindPlaceState::MindPlaceState() : State(), detailVisible(false), tabs(*this)
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
    backgroundSR->SetScale(mindPlaceBackgroundScale, mindPlaceBackgroundScale);
    backgroundGO->AddComponent(backgroundSR);
    backgroundGO->box.SetCenter(mindPlaceBackgroundCenter);
    AddObject(backgroundGO);

    // --- tab system ---
    const float offscreenY = 2000.0f;
    const float folderScaleLocal = 0.65f;

    int tab0 = tabs.AddTab(Vec2(162.0f, 110.0f),
                           "recursos/img/abaPessoasDownEsboço.png",
                           "recursos/img/abaPessoasUpEsboço.png");
    int tab1 = tabs.AddTab(Vec2(362.0f, 110.0f),
                           "recursos/img/abaPessoasDownEsboço.png",
                           "recursos/img/abaPessoasUpEsboço.png");
    int tab2 = tabs.AddTab(Vec2(562.0f, 110.0f),
                           "recursos/img/abaPessoasDownEsboço.png",
                           "recursos/img/abaPessoasUpEsboço.png");

    {
        Vec2 center(190.0f, 210.0f);
        GameObject* dragFolder = new GameObject();
        SpriteRenderer* sr = new SpriteRenderer(*dragFolder, "recursos/img/esboçoPasta.png");
        sr->SetScale(folderScaleLocal, folderScaleLocal);
        dragFolder->AddComponent(sr);
        dragFolder->AddComponent(new Draggable(*dragFolder, true));
        dragFolder->box.SetCenter(Vec2(center.x, center.y + offscreenY));
        dragFolder->GetComponent<Draggable>()->SetSpawnPosition(center);
        tabs.AddContent(tab0, AddObject(dragFolder));
    }
    {
        Vec2 center(340.0f, 210.0f);
        GameObject* interactFolder = new GameObject();
        SpriteRenderer* sr = new SpriteRenderer(*interactFolder, "recursos/img/esboçoPasta.png");
        sr->SetScale(folderScaleLocal, folderScaleLocal);
        interactFolder->AddComponent(sr);
        interactFolder->AddComponent(new Interactable(
            *interactFolder,
            Interactable::CLICK_ONLY,
            Interactable::NO_ACTOR,
            0.0f,
            [this]() { OpenInteractFolderDetail(); }
        ));
        interactFolder->box.SetCenter(Vec2(center.x, center.y + offscreenY));
        tabs.AddContent(tab0, AddObject(interactFolder));
    }

    {
        Vec2 center(190.0f, 210.0f);
        GameObject* dragFolder = new GameObject();
        SpriteRenderer* sr = new SpriteRenderer(*dragFolder, "recursos/img/esboçoPasta.png");
        sr->SetScale(folderScaleLocal, folderScaleLocal);
        dragFolder->AddComponent(sr);
        dragFolder->AddComponent(new Draggable(*dragFolder, true));
        dragFolder->box.SetCenter(Vec2(center.x, center.y + offscreenY));
        dragFolder->GetComponent<Draggable>()->SetSpawnPosition(center);
        tabs.AddContent(tab1, AddObject(dragFolder));
    }

    {
        Vec2 center(190.0f, 210.0f);
        GameObject* interactFolder = new GameObject();
        SpriteRenderer* sr = new SpriteRenderer(*interactFolder, "recursos/img/esboçoPasta.png");
        sr->SetScale(folderScaleLocal, folderScaleLocal);
        interactFolder->AddComponent(sr);
        interactFolder->AddComponent(new Interactable(
            *interactFolder,
            Interactable::CLICK_ONLY,
            Interactable::NO_ACTOR,
            0.0f,
            [this]() { OpenInteractFolderDetail(); }
        ));
        interactFolder->box.SetCenter(Vec2(center.x, center.y + offscreenY));
        tabs.AddContent(tab2, AddObject(interactFolder));
    }

    tabs.SwitchTo(0);

    StartArray();
    started = true;
}

void MindPlaceState::LoadAssets()
{
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
        if (detailVisible)
        {
            CloseDetailPanel();
        }
        else
        {
            popRequested = true;
        }
        return;
    }

    if (!detailVisible && input.MousePress(LEFT_MOUSE_BUTTON))
    {
        Vec2 mouseWorldPoint(input.GetMouseX() + Camera::pos.x,
                             input.GetMouseY() + Camera::pos.y);
        ActivateInteractableAtPoint(mouseWorldPoint);
    }

    if (!detailVisible)
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

void MindPlaceState::AddDetailSprite(const std::string &imagePath,
                                     const Vec2 &visibleCenter,
                                     float scaleX,
                                     float scaleY)
{
    GameObject* detailGO = new GameObject();
    SpriteRenderer* detailSR = new SpriteRenderer(*detailGO, imagePath);
    detailSR->SetScale(scaleX, scaleY);
    detailGO->AddComponent(detailSR);

    const Vec2 hiddenCenter(visibleCenter.x, visibleCenter.y + 1200.0f);
    detailGO->box.SetCenter(hiddenCenter);

    DetailObjectEntry entry;
    entry.object = AddObject(detailGO);
    entry.visibleCenter = visibleCenter;
    entry.hiddenCenter = hiddenCenter;
    activeDetailObjects.push_back(entry);
}

void MindPlaceState::ShowFolderContent(const std::string &imagePath,
                                       const Vec2 &visibleCenter,
                                       float scaleX,
                                       float scaleY)
{
    if (activeDetailObjects.empty())
    {
        AddDetailSprite(imagePath, visibleCenter, scaleX, scaleY);
    }

    detailVisible = true;

    for (DetailObjectEntry &entry : activeDetailObjects)
    {
        std::shared_ptr<GameObject> object = entry.object.lock();
        if (!object)
        {
            continue;
        }

        object->box.SetCenter(entry.visibleCenter);
    }
}

void MindPlaceState::EnsureInteractFolderDetail()
{
    if (!activeDetailObjects.empty())
    {
        return;
    }

    AddDetailSprite("recursos/img/EsboçoMentalSelecionado.png",
                    mindPlaceBackgroundCenter,
                    mindPlaceBackgroundScale,
                    mindPlaceBackgroundScale);
}

void MindPlaceState::OpenInteractFolderDetail()
{
    EnsureInteractFolderDetail();
    ShowFolderContent("recursos/img/EsboçoMentalSelecionado.png",
                      mindPlaceBackgroundCenter,
                      mindPlaceBackgroundScale,
                      mindPlaceBackgroundScale);
}

void MindPlaceState::CloseDetailPanel()
{
    detailVisible = false;

    for (DetailObjectEntry &entry : activeDetailObjects)
    {
        std::shared_ptr<GameObject> object = entry.object.lock();
        if (!object)
        {
            continue;
        }

        object->box.SetCenter(entry.hiddenCenter);
    }
}