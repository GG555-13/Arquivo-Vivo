#ifndef INTERACTABLE_H
#define INTERACTABLE_H

#include "Component.h"
#include "Vec2.h"

#include <functional>
#include <memory>

class Interactable : public Component
{
public:
    enum ActivationType
    {
        SPACE_ONLY,
        CLICK_ONLY,
        SPACE_OR_CLICK,
        AUTO_ENTER
    };

    enum Requirement
    {
        NO_ACTOR,
        REQUIRE_NEAR,
        REQUIRE_INSIDE_AREA
    };

    struct InteractionContext
    {
        bool hasActor;
        Vec2 actorPos;
        bool hasInteractionPoint;
        Vec2 interactionPoint;

        InteractionContext()
            : hasActor(false), actorPos(), hasInteractionPoint(false), interactionPoint()
        {
        }
    };

    Interactable(GameObject &associated,
                 ActivationType activationType,
                 Requirement requirement,
                 float interactionRadius,
                 std::function<void()> onInteract,
                 float markerOffsetY = 0.0f,
                 std::function<bool()> condition = nullptr); 

    void Start() override;
    void Update(float dt) override;
    void Render() override;

    bool IsActorNear(const Vec2 &actorPos) const;
    bool ContainsPoint(const Vec2 &worldPoint) const;
    bool IsActorInsideArea(const Vec2 &actorPos) const;
    bool CanAutoActivate(const InteractionContext &context) const;
    bool CanActivate(const InteractionContext &context) const;

    void Activate();

    void SetEnabled(bool value);
    bool IsEnabled() const;

    ActivationType GetActivationType() const;
    Requirement GetRequirement() const;

protected:
    ActivationType activationType;
    Requirement requirement;
    float interactionRadius;
    bool enabled;
    bool wasPlayerNearLastFrame;
    std::weak_ptr<GameObject> areaMarker;
    std::function<void()> onInteract;
    float markerOffsetY;

    bool MeetsRequirement(const InteractionContext &context) const;
    void UpdateAreaMarker(const InteractionContext &context);
    std::function<bool()> condition;
};

#endif