#include "Draggable.h"

#include "Camera.h"
#include "GameObject.h"
#include "InputManager.h"

Draggable *Draggable::activeDraggable = nullptr;

Draggable::Draggable(GameObject &associated)
    : Component(associated), dragging(false), dragOffset()
{
}

Draggable::~Draggable()
{
    if (activeDraggable == this)
    {
        activeDraggable = nullptr;
    }
}

void Draggable::Update(float dt)
{
    (void)dt;

    InputManager &input = InputManager::GetInstance();
    Vec2 mouseWorldPoint(input.GetMouseX() + Camera::pos.x,
                         input.GetMouseY() + Camera::pos.y);

    if (dragging)
    {
        if (input.IsMouseDown(LEFT_MOUSE_BUTTON))
        {
            associated.box.x = mouseWorldPoint.x - dragOffset.x;
            associated.box.y = mouseWorldPoint.y - dragOffset.y;
            return;
        }

        dragging = false;
        if (activeDraggable == this)
        {
            activeDraggable = nullptr;
        }
        return;
    }

    if (activeDraggable != nullptr || !input.MousePress(LEFT_MOUSE_BUTTON))
    {
        return;
    }

    if (!associated.box.Contains(mouseWorldPoint))
    {
        return;
    }

    dragging = true;
    activeDraggable = this;
    dragOffset = Vec2(mouseWorldPoint.x - associated.box.x,
                      mouseWorldPoint.y - associated.box.y);
}

void Draggable::Render()
{
}

bool Draggable::IsDragging() const
{
    return dragging;
}