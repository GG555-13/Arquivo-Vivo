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
const float folderScale = 0.65f;
const Vec2 dragFolderCenter(190.0f, 210.0f);
const Vec2 interactFolderCenter(340.0f, 210.0f);
}

MindPlaceState::MindPlaceState() : State(), detailVisible(false)
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

    GameObject* dragFolderGO = new GameObject();
    SpriteRenderer* dragFolderSR = new SpriteRenderer(*dragFolderGO, "recursos/img/esboçoPasta.png");
    dragFolderSR->SetScale(folderScale, folderScale);
    dragFolderGO->AddComponent(dragFolderSR);
    dragFolderGO->AddComponent(new Draggable(*dragFolderGO, true));
    dragFolderGO->box.SetCenter(dragFolderCenter);
    dragFolderGO->GetComponent<Draggable>()->SetSpawnPosition(dragFolderGO->box.Center());
    AddObject(dragFolderGO);

    GameObject* interactFolderGO = new GameObject();
    SpriteRenderer* interactFolderSR = new SpriteRenderer(*interactFolderGO, "recursos/img/esboçoPasta.png");
    interactFolderSR->SetScale(folderScale, folderScale);
    interactFolderGO->AddComponent(interactFolderSR);
    interactFolderGO->AddComponent(new Interactable(
        *interactFolderGO,
        Interactable::CLICK_ONLY,
        Interactable::NO_ACTOR,
        0.0f,
        [this]()
        {
            OpenInteractFolderDetail();
        }
    ));
    interactFolderGO->box.SetCenter(interactFolderCenter);
    AddObject(interactFolderGO);

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