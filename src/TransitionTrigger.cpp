#include "TransitionTrigger.h"
#include "GameObject.h"
#include "Character.h"
#include "Game.h"
#include "SpriteRenderer.h"

TransitionTrigger::TransitionTrigger(GameObject &associated,
                                     ActivationType activationType,
                                     float interactionRadius,
                                     std::function<void()> onActivate)
    : Component(associated),
      activationType(activationType),
      interactionRadius(interactionRadius),
      enabled(true),
            wasPlayerNearLastFrame(false),
      onActivate(onActivate)
{
}

void TransitionTrigger::Start()
{
        GameObject* markerGo = new GameObject();
        SpriteRenderer* sprite = new SpriteRenderer(*markerGo, "recursos/img/Bullet.png");
        markerGo->AddComponent(sprite);
        markerGo->box.w = sprite->GetWidth();
        markerGo->box.h = sprite->GetHeight();
        markerGo->box.x = -10000.0f;
        markerGo->box.y = -10000.0f;
        areaMarker = Game::GetInstance().GetCurrentState().AddObject(markerGo);
}

void TransitionTrigger::Update(float dt)
{
    (void)dt;

    if (!enabled)
    {
        wasPlayerNearLastFrame = false;
        UpdateAreaMarker(Vec2(-10000.0f, -10000.0f));
        return;
    }

    if (Character::player != nullptr)
    {
        UpdateAreaMarker(Character::player->GetPosition());
    }
    else
    {
        UpdateAreaMarker(Vec2(-10000.0f, -10000.0f));
    }

    if (activationType != AUTO_ENTER)
    {
        wasPlayerNearLastFrame = false;
        return;
    }

    if (Character::player == nullptr)
    {
        wasPlayerNearLastFrame = false;
        return;
    }

    const Vec2 playerPos = Character::player->GetPosition();
    const bool isPlayerNear = CanAutoActivate(playerPos);

    if (isPlayerNear && !wasPlayerNearLastFrame)
    {
        Activate();
    }

    wasPlayerNearLastFrame = isPlayerNear;
}

void TransitionTrigger::Render()
{
}

bool TransitionTrigger::IsPlayerNear(const Vec2 &playerPos) const
{
    return playerPos.Distance(associated.box.Center()) <= interactionRadius;
}

bool TransitionTrigger::ContainsPoint(const Vec2 &worldPoint) const
{
    return associated.box.Contains(worldPoint);
}

bool TransitionTrigger::IsPlayerInsideArea(const Vec2 &playerPos) const
{
    return ContainsPoint(playerPos);
}

bool TransitionTrigger::CanAutoActivate(const Vec2 &playerPos) const
{
    if (!enabled)
    {
        return false;
    }

    if (activationType != AUTO_ENTER)
    {
        return false;
    }

    return IsPlayerNear(playerPos);
}

bool TransitionTrigger::CanActivateWithSpace(const Vec2 &playerPos) const
{
    if (!enabled)
    {
        return false;
    }

    if (activationType != SPACE_ONLY && activationType != SPACE_OR_CLICK)
    {
        return false;
    }

    return IsPlayerNear(playerPos);
}

bool TransitionTrigger::CanActivateWithClick(const Vec2 &playerPos, const Vec2 &worldPoint) const
{
    if (!enabled)
    {
        return false;
    }

    if (activationType != CLICK_ONLY && activationType != SPACE_OR_CLICK)
    {
        return false;
    }

    return IsPlayerNear(playerPos) && ContainsPoint(worldPoint);
}

void TransitionTrigger::Activate()
{
    if (!enabled)
    {
        return;
    }

    if (onActivate)
    {
        onActivate();
    }
}

void TransitionTrigger::SetEnabled(bool value)
{
    enabled = value;

    if (!enabled)
    {
        wasPlayerNearLastFrame = false;
    }
}

bool TransitionTrigger::IsEnabled() const
{
    return enabled;
}

TransitionTrigger::ActivationType TransitionTrigger::GetActivationType() const
{
    return activationType;
}

void TransitionTrigger::UpdateAreaMarker(const Vec2 &playerPos)
{
    std::shared_ptr<GameObject> marker = areaMarker.lock();
    if (!marker)
    {
        return;
    }

    if (!enabled || !IsPlayerInsideArea(playerPos))
    {
        marker->box.x = -10000.0f;
        marker->box.y = -10000.0f;
        return;
    }

    marker->box.x = associated.box.Center().x - marker->box.w / 2.0f;
    marker->box.y = associated.box.y - marker->box.h - 12.0f;
}