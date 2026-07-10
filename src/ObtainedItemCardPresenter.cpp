#include "ObtainedItemCardPresenter.h"

#include "Camera.h"
#include "DetailContent.h"
#include "GameObject.h"
#include "InputManager.h"
#include "Inventory.h"
#include "InventoryCatalog.h"
#include "InventoryEntryDefinition.h"
#include "SpriteRenderer.h"
#include "State.h"
#include "Text.h"

#include <algorithm>

namespace
{
const Vec2 CARD_OPEN_CENTER(600.0f, 450.0f);
const Vec2 CARD_START_CENTER(600.0f, 1050.0f);
const Vec2 CARD_HIDDEN_CENTER(600.0f, 1800.0f);
const float CARD_ANIMATION_DURATION = 0.25f;

float SmoothStep(float value)
{
    return value * value * (3.0f - 2.0f * value);
}

void MoveTo(std::weak_ptr<GameObject> object, const Vec2 &position)
{
    if (auto value = object.lock())
    {
        value->box.SetCenter(position);
    }
}
}

ObtainedItemCardPresenter::ObtainedItemCardPresenter(State &owner)
    : state(owner),
      mode(Mode::Idle),
      elapsed(0.0f),
      cardCenter(CARD_HIDDEN_CENTER),
      closingStartCenter(CARD_OPEN_CENTER)
{
}

bool ObtainedItemCardPresenter::IsActive() const
{
    return mode != Mode::Idle || Inventory::HasPendingNotifications();
}

void ObtainedItemCardPresenter::Update(float dt)
{
    if (mode == Mode::Idle)
    {
        OpenNext();
        return;
    }

    InputManager &input = InputManager::GetInstance();
    if (mode != Mode::Closing && (input.KeyPress(ESCAPE_KEY) || input.KeyPress(MIND_PLACE_KEY)))
    {
        Close();
    }

    if (mode == Mode::Visible)
    {
        return;
    }

    elapsed += dt;
    float progress = elapsed / CARD_ANIMATION_DURATION;
    if (progress >= 1.0f)
    {
        progress = 1.0f;
    }

    const float eased = SmoothStep(progress);
    if (mode == Mode::Opening)
    {
        cardCenter = Vec2(
            CARD_START_CENTER.x + (CARD_OPEN_CENTER.x - CARD_START_CENTER.x) * eased,
            CARD_START_CENTER.y + (CARD_OPEN_CENTER.y - CARD_START_CENTER.y) * eased);
        if (progress >= 1.0f)
        {
            mode = Mode::Visible;
        }
    }
    else if (mode == Mode::Closing)
    {
        cardCenter = Vec2(
            closingStartCenter.x + (CARD_HIDDEN_CENTER.x - closingStartCenter.x) * eased,
            closingStartCenter.y + (CARD_HIDDEN_CENTER.y - closingStartCenter.y) * eased);
        if (progress >= 1.0f)
        {
            mode = Mode::Idle;
            cardCenter = CARD_HIDDEN_CENTER;
        }
    }

    PositionObjects(cardCenter);
}

void ObtainedItemCardPresenter::EnsureObjects()
{
    if (!cardObject.expired())
    {
        return;
    }

    GameObject *card = new GameObject();
    SpriteRenderer *cardSprite = new SpriteRenderer(*card, "recursos/img/estrutura_info.png");
    cardSprite->SetScale(1.30f, 1.30f);
    cardSprite->SetUseSourceFrameOffset(false);
    card->AddComponent(cardSprite);
    card->box.SetCenter(CARD_HIDDEN_CENTER);
    cardObject = state.AddObject(card);

    imageObject = state.AddObject(new GameObject());

    GameObject *title = new GameObject();
    title->AddComponent(new Text(*title, "recursos/font/neodgm.ttf", 30, Text::BLENDED, "", {35, 28, 23, 255}, 460));
    titleObject = state.AddObject(title);

    GameObject *description = new GameObject();
    Text *descriptionText = new Text(*description, "recursos/font/neodgm.ttf", 20, Text::BLENDED, "", {50, 38, 30, 255}, 460);
    descriptionText->SetViewportHeight(168);
    description->AddComponent(descriptionText);
    descriptionObject = state.AddObject(description);

    PositionObjects(CARD_HIDDEN_CENTER);
}

bool ObtainedItemCardPresenter::OpenNext()
{
    while (Inventory::HasPendingNotifications())
    {
        const std::string entryId = Inventory::PopPendingNotification();
        if (!InventoryCatalog::Find(entryId))
        {
            continue;
        }

        EnsureObjects();
        SetContent(entryId);
        mode = Mode::Opening;
        elapsed = 0.0f;
        cardCenter = CARD_START_CENTER;
        PositionObjects(cardCenter);
        return true;
    }

    PositionObjects(CARD_HIDDEN_CENTER);
    return false;
}

void ObtainedItemCardPresenter::Close()
{
    if (mode == Mode::Idle || mode == Mode::Closing)
    {
        return;
    }

    mode = Mode::Closing;
    elapsed = 0.0f;
    closingStartCenter = cardCenter;
}

void ObtainedItemCardPresenter::SetContent(const std::string &entryId)
{
    const InventoryEntryDefinition *definition = InventoryCatalog::Find(entryId);
    if (!definition)
    {
        return;
    }

    DetailContent content{definition->name, definition->description, definition->detailImage};
    if (definition->kind == InventoryEntryKind::DialogueFolder)
    {
        content.description = Inventory::GetDialogueHistory(definition->characterId);
    }

    if (auto image = imageObject.lock())
    {
        if (auto old = image->GetComponent<SpriteRenderer>())
        {
            image->RemoveComponent(old);
        }
        if (!content.image.empty())
        {
            SpriteRenderer *sprite = new SpriteRenderer(*image, content.image);
            const float width = sprite->GetWidth();
            const float height = sprite->GetHeight();
            if (width > 0.0f && height > 0.0f)
            {
                const float scale = std::min(225.0f / width, 225.0f / height);
                sprite->SetScale(scale, scale);
            }
            sprite->SetUseSourceFrameOffset(false);
            image->AddComponent(sprite);
        }
    }

    if (auto title = titleObject.lock())
    {
        title->GetComponent<Text>()->SetText(content.title);
    }
    if (auto description = descriptionObject.lock())
    {
        description->GetComponent<Text>()->SetText(content.description);
    }
}

void ObtainedItemCardPresenter::PositionObjects(const Vec2 &center)
{
    const Vec2 screenCenter(Camera::pos.x + center.x, Camera::pos.y + center.y);
    MoveTo(cardObject, screenCenter);
    MoveTo(imageObject, Vec2(screenCenter.x, screenCenter.y - 140.0f));
    MoveTo(titleObject, Vec2(screenCenter.x, screenCenter.y + 25.0f));
    MoveTo(descriptionObject, Vec2(screenCenter.x + 6.0f, screenCenter.y + 155.0f));
}
