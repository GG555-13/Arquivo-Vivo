#include "Interactable.h"

#include "Character.h"
#include "Game.h"
#include "GameObject.h"
#include "SpriteRenderer.h"

Interactable::Interactable(GameObject &associated,
                           ActivationType activationType,
                           float interactionRadius,
                           std::function<void()> onInteract)
    : Component(associated),
      activationType(activationType),
      interactionRadius(interactionRadius),
      enabled(true),
      wasPlayerNearLastFrame(false),
      onInteract(onInteract)
{
}

void Interactable::Start()
{
    GameObject *markerGo = new GameObject();
    SpriteRenderer *sprite = new SpriteRenderer(*markerGo, "recursos/img/Bullet.png");
    markerGo->AddComponent(sprite);
    markerGo->box.w = sprite->GetWidth();
    markerGo->box.h = sprite->GetHeight();
    markerGo->box.x = -10000.0f;
    markerGo->box.y = -10000.0f;
    areaMarker = Game::GetInstance().GetCurrentState().AddObject(markerGo);
}

void Interactable::Update(float dt)
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

void Interactable::Render()
{
}

bool Interactable::IsPlayerNear(const Vec2 &playerPos) const
{
    return playerPos.Distance(associated.box.Center()) <= interactionRadius;
}

bool Interactable::ContainsPoint(const Vec2 &worldPoint) const
{
    return associated.box.Contains(worldPoint);
}

bool Interactable::IsPlayerInsideArea(const Vec2 &playerPos) const
{
    return ContainsPoint(playerPos);
}

bool Interactable::CanAutoActivate(const Vec2 &playerPos) const
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

bool Interactable::CanActivate(const Vec2 &playerPos, bool hasInteractionPoint, const Vec2 &interactionPoint) const
{
    if (!enabled)
    {
        return false;
    }

    if (!IsPlayerNear(playerPos))
    {
        return false;
    }

    if (activationType == AUTO_ENTER)
    {
        return false;
    }

    if (!hasInteractionPoint)
    {
        return activationType == SPACE_ONLY || activationType == SPACE_OR_CLICK;
    }

    if (activationType == SPACE_ONLY)
    {
        return false;
    }

    return ContainsPoint(interactionPoint);
}

void Interactable::Activate()
{
    if (!enabled)
    {
        return;
    }

    if (onInteract)
    {
        onInteract();
    }
}

void Interactable::SetEnabled(bool value)
{
    enabled = value;

    if (!enabled)
    {
        wasPlayerNearLastFrame = false;
    }
}

bool Interactable::IsEnabled() const
{
    return enabled;
}

Interactable::ActivationType Interactable::GetActivationType() const
{
    return activationType;
}

void Interactable::UpdateAreaMarker(const Vec2 &playerPos)
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