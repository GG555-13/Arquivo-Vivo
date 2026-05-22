#ifndef TRANSITION_TRIGGER_H
#define TRANSITION_TRIGGER_H

#include "Component.h"
#include "Vec2.h"
#include <functional>
#include <memory>

class TransitionTrigger : public Component
{
public:
    enum ActivationType
    {
        SPACE_ONLY,
        CLICK_ONLY,
        SPACE_OR_CLICK,
        AUTO_ENTER
    };

    TransitionTrigger(GameObject &associated,
                      ActivationType activationType,
                      float interactionRadius,
                      std::function<void()> onActivate);

    void Start() override;
    void Update(float dt) override;
    void Render() override;

    bool IsPlayerNear(const Vec2 &playerPos) const;
    bool ContainsPoint(const Vec2 &worldPoint) const;
    bool IsPlayerInsideArea(const Vec2 &playerPos) const;
    bool CanAutoActivate(const Vec2 &playerPos) const;
    bool CanActivateWithSpace(const Vec2 &playerPos) const;
    bool CanActivateWithClick(const Vec2 &playerPos, const Vec2 &worldPoint) const;

    void Activate();

    void SetEnabled(bool value);
    bool IsEnabled() const;

    ActivationType GetActivationType() const;

private:
    ActivationType activationType;
    float interactionRadius;
    bool enabled;
    bool wasPlayerNearLastFrame;
    std::weak_ptr<GameObject> areaMarker;
    std::function<void()> onActivate;

    void UpdateAreaMarker(const Vec2 &playerPos);
};

#endif