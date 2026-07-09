#include "DetailPanel.h"

#include "GameObject.h"
#include "DetailContent.h"
#include "SpriteRenderer.h"
#include "State.h"
#include "Text.h"

#include <algorithm>

namespace {
const Vec2 DETAIL_OPEN_CENTER(600.0f, 450.0f);
const Vec2 DETAIL_START_CENTER(600.0f, 1050.0f);
const Vec2 DETAIL_HIDDEN_CENTER(600.0f, 1150.0f);
const float DETAIL_OPEN_DURATION = 0.25f;

void MoveTo(std::weak_ptr<GameObject> object, const Vec2 &position) {
    if (auto value = object.lock()) value->box.SetCenter(position);
}

float SmoothStep(float value) {
    return value * value * (3.0f - 2.0f * value);
}

}

DetailPanel::DetailPanel(State &owner,
                         const Vec2 &defaultCenter,
                         float defaultScale,
                         const std::string &defaultSprite)
    : state(owner),
      visibleCenter(defaultCenter),
      hiddenCenter(defaultCenter.x, defaultCenter.y + HIDDEN_OFFSET_Y),
      visible(false),
      defaultCenter(defaultCenter),
      defaultScale(defaultScale),
      defaultSprite(defaultSprite),
      opening(false),
      closing(false),
      openingElapsed(0.0f),
      detailCenter(DETAIL_HIDDEN_CENTER),
      closingStartCenter(DETAIL_OPEN_CENTER)
{
}

bool DetailPanel::IsVisible() const
{
    return visible;
}

void DetailPanel::Update(float dt)
{
    if (!visible || (!opening && !closing)) return;

    openingElapsed += dt;
    float progress = openingElapsed / DETAIL_OPEN_DURATION;
    if (progress >= 1.0f) {
        progress = 1.0f;
    }

    const float eased = SmoothStep(progress);
    if (opening) {
        detailCenter = Vec2(
            DETAIL_START_CENTER.x + (DETAIL_OPEN_CENTER.x - DETAIL_START_CENTER.x) * eased,
            DETAIL_START_CENTER.y + (DETAIL_OPEN_CENTER.y - DETAIL_START_CENTER.y) * eased
        );
        if (progress >= 1.0f) opening = false;
    } else {
        detailCenter = Vec2(
            closingStartCenter.x + (DETAIL_HIDDEN_CENTER.x - closingStartCenter.x) * eased,
            closingStartCenter.y + (DETAIL_HIDDEN_CENTER.y - closingStartCenter.y) * eased
        );
        if (progress >= 1.0f) {
            closing = false;
            visible = false;
        }
    }
    PositionDetailObjects(detailCenter);
}

void DetailPanel::Scroll(int amount)
{
    if (!visible) return;
    if (auto description = descriptionObject.lock()) {
        if (Text *text = description->GetComponent<Text>()) {
            text->ScrollVertical(amount);
        }
    }
}

void DetailPanel::Open()
{
    if (overlayObject.expired())
    {
        GameObject *go = new GameObject();
        SpriteRenderer *sr = new SpriteRenderer(*go, defaultSprite);
        sr->SetScale(defaultScale, defaultScale);
        go->AddComponent(sr);
        go->box.SetCenter(hiddenCenter);
        overlayObject = state.AddObject(go);
    }

    visible = true;
    std::shared_ptr<GameObject> overlay = overlayObject.lock();
    if (overlay)
    {
        overlay->box.SetCenter(visibleCenter);
    }
}

void DetailPanel::Open(const DetailContent &content)
{
    if (overlayObject.expired()) {
        GameObject *card = new GameObject();
        SpriteRenderer *sprite = new SpriteRenderer(*card, "recursos/img/estrutura_info.png");
        sprite->SetScale(1.30f, 1.30f);
        sprite->SetUseSourceFrameOffset(false);
        card->AddComponent(sprite);
        card->box.SetCenter(hiddenCenter);
        overlayObject = state.AddObject(card);

        GameObject *image = new GameObject();
        contentImageObject = state.AddObject(image);

        GameObject *title = new GameObject();
        title->AddComponent(new Text(*title, "recursos/font/neodgm.ttf", 30, Text::BLENDED, "", {35, 28, 23, 255}, 460));
        titleObject = state.AddObject(title);

        GameObject *description = new GameObject();
        Text *descriptionText = new Text(*description, "recursos/font/neodgm.ttf", 20, Text::BLENDED, "", {50, 38, 30, 255}, 460);
        descriptionText->SetViewportHeight(168);
        description->AddComponent(descriptionText);
        descriptionObject = state.AddObject(description);
    }

    if (auto image = contentImageObject.lock()) {
        if (auto old = image->GetComponent<SpriteRenderer>()) image->RemoveComponent(old);
        if (!content.image.empty()) {
            SpriteRenderer *sprite = new SpriteRenderer(*image, content.image);
            const float width = sprite->GetWidth();
            const float height = sprite->GetHeight();
            if (width > 0.0f && height > 0.0f) {
                const float scale = std::min(225.0f / width, 225.0f / height);
                sprite->SetScale(scale, scale);
            }
            sprite->SetUseSourceFrameOffset(false);
            image->AddComponent(sprite);
        }
    }
    if (auto title = titleObject.lock()) title->GetComponent<Text>()->SetText(content.title);
    if (auto description = descriptionObject.lock()) description->GetComponent<Text>()->SetText(content.description);

    visible = true;
    opening = true;
    closing = false;
    openingElapsed = 0.0f;
    detailCenter = DETAIL_START_CENTER;
    PositionDetailObjects(detailCenter);
}

void DetailPanel::Close()
{
    if (!visible || closing) return;

    opening = false;
    closing = true;
    openingElapsed = 0.0f;
    closingStartCenter = detailCenter;
}

void DetailPanel::PositionDetailObjects(const Vec2 &center)
{
    MoveTo(overlayObject, center);
    MoveTo(contentImageObject, Vec2(center.x, center.y - 140.0f));
    MoveTo(titleObject, Vec2(center.x, center.y + 25.0f));
    MoveTo(descriptionObject, Vec2(center.x + 6.0f, center.y + 155.0f));
}
