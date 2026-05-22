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

    Interactable(GameObject &associated,
                 ActivationType activationType,
                 float interactionRadius,
                 std::function<void()> onInteract);

    void Start() override;
    void Update(float dt) override;
    void Render() override;

    bool IsPlayerNear(const Vec2 &playerPos) const;
    bool ContainsPoint(const Vec2 &worldPoint) const;
    bool IsPlayerInsideArea(const Vec2 &playerPos) const;
    bool CanAutoActivate(const Vec2 &playerPos) const;
    bool CanActivate(const Vec2 &playerPos, bool hasInteractionPoint, const Vec2 &interactionPoint) const;

    void Activate();

    void SetEnabled(bool value);
    bool IsEnabled() const;

    ActivationType GetActivationType() const;

protected:
    ActivationType activationType;
    float interactionRadius;
    bool enabled;
    bool wasPlayerNearLastFrame;
    std::weak_ptr<GameObject> areaMarker;
    std::function<void()> onInteract;

    void UpdateAreaMarker(const Vec2 &playerPos);
};

#endif