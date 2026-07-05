#ifndef TEMPORARY_H
#define TEMPORARY_H

#include "Component.h"
#include "Timer.h"

class GameObject;

class Temporary : public Component
{
public:
    Temporary(GameObject& associated, float timeLimit);
    
    void Start() override;
    void Update(float dt) override;
    void Render() override;
    void NotifyCollision(GameObject& other) override;

private:
    Timer lifeTimer;
    float timeLimit;
};

#endif