#include "GameObject.h"
#include "Component.h"

void Component::Start() {}

GameObject::GameObject() : angleDeg(0.0), sortingY(0.0f), started(false), isDead(false) {}

GameObject::~GameObject()
{
    components.clear();
}

void GameObject::Start()
{
    started = true;
    for (size_t i = 0; i < components.size(); ++i)
    {
        components[i]->Start();
    }
}

void Component::NotifyCollision(GameObject &other) {}

Component::Component(GameObject &associated) : associated(associated) {}

void GameObject::Update(float dt)
{
    sortingY = box.Center().y;

    for (const auto &component : components)
    {
        component->Update(dt);
    }
}

void GameObject::Render()
{
    for (const auto &component : components)
    {
        component->Render();
    }
}

bool GameObject::IsDead() const
{
    return isDead;
}

void GameObject::RequestDelete()
{
    isDead = true;
}

void GameObject::NotifyCollision(GameObject &other)
{
    for (size_t i = 0; i < components.size(); ++i)
    {
        components[i]->NotifyCollision(other);
    }
}

void GameObject::AddComponent(Component *cpt)
{
    components.emplace_back(cpt);
    if (started)
    {
        cpt->Start();
    }
}

void GameObject::RemoveComponent(Component *cpt)
{
    components.erase(std::remove_if(components.begin(), components.end(), [&](const std::unique_ptr<Component> &p)
                                    { return p.get() == cpt; }),
                     components.end());
}