#ifndef COMPONENT_H
#define COMPONENT_H

class GameObject;

class Component
{
public:
    virtual ~Component() = default;
    virtual void Update(float dt) = 0;
    virtual void Render() = 0;
    virtual void NotifyCollision(GameObject& other);
    virtual void Start();

protected:
    explicit Component(GameObject &associated);
    GameObject &associated;
};

#endif