#include "Interactable.h"

#include "Character.h"
#include "Game.h"
#include "GameObject.h"
#include "SpriteRenderer.h"

Interactable::Interactable(GameObject &associated,
                           ActivationType activationType,
                                                     Requirement requirement,
                           float interactionRadius,
                           std::function<void()> onInteract)
    : Component(associated),
      activationType(activationType),
            requirement(requirement),
      interactionRadius(interactionRadius),
      enabled(true),
      wasPlayerNearLastFrame(false),
      onInteract(onInteract)
{
}

void Interactable::Start()
{
    GameObject *markerGo = new GameObject();
    SpriteRenderer *sprite = new SpriteRenderer(*markerGo, "recursos/img/interactible_indicator.png");
    sprite->SetScale(0.1f, 0.1f);
    sprite->SetUseSourceFrameOffset(false);
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
        UpdateAreaMarker(InteractionContext());
        return;
    }

    InteractionContext context;
    if (Character::player != nullptr)
    {
        context.hasActor = true;
        context.actorPos = Character::player->GetPosition();
    }

    UpdateAreaMarker(context);

    if (activationType != AUTO_ENTER)
    {
        wasPlayerNearLastFrame = false;
        return;
    }

    const bool isActorEligible = CanAutoActivate(context);

    if (isActorEligible && !wasPlayerNearLastFrame)
    {
        Activate();
    }

    wasPlayerNearLastFrame = isActorEligible;
}

void Interactable::Render()
{
}

bool Interactable::IsActorNear(const Vec2 &actorPos) const
{
    return actorPos.Distance(associated.box.Center()) <= interactionRadius;
}

bool Interactable::ContainsPoint(const Vec2 &worldPoint) const
{
    return associated.box.Contains(worldPoint);
}

bool Interactable::IsActorInsideArea(const Vec2 &actorPos) const
{
    return ContainsPoint(actorPos);
}

bool Interactable::CanAutoActivate(const InteractionContext &context) const
{
    if (!enabled)
    {
        return false;
    }

    if (activationType != AUTO_ENTER)
    {
        return false;
    }

    if (requirement == NO_ACTOR)
    {
        return false;
    }

    return MeetsRequirement(context);
}

bool Interactable::CanActivate(const InteractionContext &context) const
{
    if (!enabled)
    {
        return false;
    }

    if (activationType == AUTO_ENTER)
    {
        return false;
    }

    if (!MeetsRequirement(context))
    {
        return false;
    }

    if (!context.hasInteractionPoint)
    {
        return activationType == SPACE_ONLY || activationType == SPACE_OR_CLICK;
    }

    if (activationType == SPACE_ONLY)
    {
        return false;
    }

    return ContainsPoint(context.interactionPoint);
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

Interactable::Requirement Interactable::GetRequirement() const
{
    return requirement;
}

bool Interactable::MeetsRequirement(const InteractionContext &context) const
{
    if (requirement == NO_ACTOR)
    {
        return true;
    }

    if (!context.hasActor)
    {
        return false;
    }

    if (requirement == REQUIRE_INSIDE_AREA)
    {
        return IsActorInsideArea(context.actorPos);
    }

    return IsActorNear(context.actorPos);
}

void Interactable::UpdateAreaMarker(const InteractionContext &context)
{
    std::shared_ptr<GameObject> marker = areaMarker.lock();
    if (!marker)
    {
        return;
    }

    bool shouldShow = false;
    if (enabled && context.hasActor)
    {
        if (requirement == REQUIRE_INSIDE_AREA)
        {
            shouldShow = IsActorInsideArea(context.actorPos);
        }
        else if (requirement == REQUIRE_NEAR)
        {
            shouldShow = IsActorNear(context.actorPos);
        }
    }

    if (!shouldShow)
    {
        marker->box.x = -10000.0f;
        marker->box.y = -10000.0f;
        return;
    }

    marker->box.x = associated.box.Center().x - marker->box.w / 2.0f;
    marker->box.y = associated.box.Center().y - interactionRadius - marker->box.h - 8.0f;
}