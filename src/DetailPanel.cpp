#include "DetailPanel.h"

#include "GameObject.h"
#include "SpriteRenderer.h"
#include "State.h"

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

void DetailPanel::Close()
{
    visible = false;
    std::shared_ptr<GameObject> overlay = overlayObject.lock();
    if (overlay)
    {
        overlay->box.SetCenter(hiddenCenter);
    }
}