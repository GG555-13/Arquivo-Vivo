#ifndef DRAGGABLE_H
#define DRAGGABLE_H

#include "Component.h"
#include "Vec2.h"

class Draggable : public Component
{
public:
    explicit Draggable(GameObject &associated);
    ~Draggable() override;

    void Update(float dt) override;
    void Render() override;

    bool IsDragging() const;

private:
    static Draggable *activeDraggable;

    bool dragging;
    Vec2 dragOffset;
};

#endif