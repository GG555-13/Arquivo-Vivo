#ifndef GAMEOBJECT_H
#define GAMEOBJECT_H

#include "Rect.h"
#include <vector>
#include <string>
#include <memory>
#include <algorithm>

class Component;

class GameObject
{
public:
    GameObject();
    ~GameObject();

    void Start();
    void Update(float dt);
    void Render();
    bool IsDead() const;
    void RequestDelete();

    void AddComponent(Component *cpt);
    void RemoveComponent(Component *cpt);
    void NotifyCollision(GameObject& other);

    Rect box;
    double angleDeg;
    float sortingY;

    template <typename T>
    T *GetComponent()
    {
        for (const auto &component : components)
        {
            T *result = dynamic_cast<T *>(component.get());
            if (result)
            {
                return result;
            }
        }
        return nullptr;
    }

private:
    bool started;
    bool isDead;
    std::vector<std::unique_ptr<Component>> components;
};

#endif