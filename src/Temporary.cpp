#include "Temporary.h"
#include "GameObject.h"

Temporary::Temporary(GameObject& associated, float timeLimit)
    : Component(associated), timeLimit(timeLimit)
{
}

void Temporary::Start()
{
}

void Temporary::Update(float dt)
{
    lifeTimer.Update(dt);

    if (lifeTimer.Get() >= timeLimit)
    {
        associated.RequestDelete();
    }
}

void Temporary::Render()
{
}

void Temporary::NotifyCollision(GameObject& other)
{
}