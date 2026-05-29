#include "Draggable.h"

#include "Camera.h"
#include "GameObject.h"
#include "InputManager.h"

Draggable *Draggable::activeDraggable = nullptr;

Draggable::Draggable(GameObject &associated, bool returnToSpawnOnRelease)
    : Component(associated),
      dragging(false),
      returnToSpawnOnRelease(returnToSpawnOnRelease),
    spawnPosition(associated.box.Center()),
      dragOffset(),
      onRelease()
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

        FinishDrag();
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

void Draggable::SetReturnToSpawnOnRelease(bool value)
{
    returnToSpawnOnRelease = value;
}

void Draggable::SetSpawnPosition(const Vec2 &position)
{
    spawnPosition = position;
}

Vec2 Draggable::GetSpawnPosition() const
{
    return spawnPosition;
}

void Draggable::ResetToSpawn()
{
    associated.box.SetCenter(spawnPosition);
}

void Draggable::SetOnRelease(std::function<bool(const Vec2 &releasePoint)> callback)
{
    onRelease = callback;
}

void Draggable::FinishDrag()
{
    dragging = false;
    if (activeDraggable == this)
    {
        activeDraggable = nullptr;
    }

    bool dropAccepted = false;
    if (onRelease)
    {
        dropAccepted = onRelease(associated.box.Center());
    }

    if (returnToSpawnOnRelease && !dropAccepted)
    {
        ResetToSpawn();
    }
}