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

MindPlaceState::MindPlaceState()
        : State(),
            tabs(*this),
            detailPanel(*this,
                                    mindPlaceBackgroundCenter,
                                    mindPlaceBackgroundScale,
                                    "recursos/img/EsboçoMentalSelecionado.png")
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

    int tab0 = tabs.AddTab(Vec2(162.0f, 110.0f),
                           "recursos/img/abaPessoasDownEsboço.png",
                           "recursos/img/abaPessoasUpEsboço.png");
    int tab1 = tabs.AddTab(Vec2(362.0f, 110.0f),
                           "recursos/img/abaPessoasDownEsboço.png",
                           "recursos/img/abaPessoasUpEsboço.png");
    int tab2 = tabs.AddTab(Vec2(562.0f, 110.0f),
                           "recursos/img/abaPessoasDownEsboço.png",
                           "recursos/img/abaPessoasUpEsboço.png");

    tabs.AddContent(tab0, CreateFolder(Vec2(190.0f, 210.0f), nullptr));
    tabs.AddContent(tab0, CreateFolder(Vec2(340.0f, 210.0f)));
    tabs.AddContent(tab1, CreateFolder(Vec2(190.0f, 210.0f), nullptr));
    tabs.AddContent(tab2, CreateFolder(Vec2(190.0f, 210.0f)));

    tabs.SwitchTo(0);

    StartArray();
    started = true;
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
