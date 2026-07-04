#ifndef DRAGGABLE_H
#define DRAGGABLE_H

#include "Component.h"
#include "Vec2.h"

#include <functional>

class Draggable : public Component
{
public:
    explicit Draggable(GameObject &associated, bool returnToSpawnOnRelease = false);
    ~Draggable() override;

    void Update(float dt) override;
    void Render() override;

    bool IsDragging() const;
    void SetReturnToSpawnOnRelease(bool value);
    void SetSpawnPosition(const Vec2 &position);
    Vec2 GetSpawnPosition() const;
    void ResetToSpawn();
    void SetOnRelease(std::function<bool(const Vec2 &releasePoint)> callback);
    void SetEnabled(bool value);

private:
    static Draggable *activeDraggable;

    void FinishDrag();

    bool dragging;
    bool returnToSpawnOnRelease;
    Vec2 spawnPosition;
    Vec2 dragOffset;
    std::function<bool(const Vec2 &releasePoint)> onRelease;
    bool enabled;
};

#endif
