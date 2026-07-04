#include "DetailPanel.h"

#include "GameObject.h"
#include "DetailContent.h"
#include "SpriteRenderer.h"
#include "State.h"
#include "Text.h"

namespace {
void MoveTo(std::weak_ptr<GameObject> object, const Vec2 &position) {
    if (auto value = object.lock()) value->box.SetCenter(position);
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
      defaultSprite(defaultSprite)
{
}

bool DetailPanel::IsVisible() const
{
    return visible;
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
        sprite->SetScale(0.78f, 0.78f);
        card->AddComponent(sprite);
        card->box.SetCenter(hiddenCenter);
        overlayObject = state.AddObject(card);

        GameObject *image = new GameObject();
        contentImageObject = state.AddObject(image);

        GameObject *title = new GameObject();
        title->AddComponent(new Text(*title, "recursos/font/neodgm.ttf", 25, Text::BLENDED, "", {35, 28, 23, 255}, 260));
        titleObject = state.AddObject(title);

        GameObject *description = new GameObject();
        description->AddComponent(new Text(*description, "recursos/font/neodgm.ttf", 17, Text::BLENDED, "", {45, 38, 30, 255}, 280));
        descriptionObject = state.AddObject(description);
    }

    if (auto image = contentImageObject.lock()) {
        if (auto old = image->GetComponent<SpriteRenderer>()) image->RemoveComponent(old);
        if (!content.image.empty()) {
            SpriteRenderer *sprite = new SpriteRenderer(*image, content.image);
            const float width = sprite->GetWidth();
            if (width > 0.0f) sprite->SetScale(135.0f / width, 135.0f / width);
            sprite->SetUseSourceFrameOffset(false);
            image->AddComponent(sprite);
        }
    }
    if (auto title = titleObject.lock()) title->GetComponent<Text>()->SetText(content.title);
    if (auto description = descriptionObject.lock()) description->GetComponent<Text>()->SetText(content.description);

    visible = true;
    MoveTo(overlayObject, visibleCenter);
    MoveTo(contentImageObject, Vec2(visibleCenter.x, visibleCenter.y - 95.0f));
    MoveTo(titleObject, Vec2(visibleCenter.x, visibleCenter.y + 5.0f));
    MoveTo(descriptionObject, Vec2(visibleCenter.x, visibleCenter.y + 78.0f));
}

void DetailPanel::Close()
{
    visible = false;
    std::shared_ptr<GameObject> overlay = overlayObject.lock();
    if (overlay)
    {
        overlay->box.SetCenter(hiddenCenter);
    }
    MoveTo(contentImageObject, Vec2(hiddenCenter.x, hiddenCenter.y - 95.0f));
    MoveTo(titleObject, Vec2(hiddenCenter.x, hiddenCenter.y + 5.0f));
    MoveTo(descriptionObject, Vec2(hiddenCenter.x, hiddenCenter.y + 78.0f));
}
