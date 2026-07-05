#ifndef PLAYERCONTROLLER_H
#define PLAYERCONTROLLER_H

#include "Component.h"

class PlayerController : public Component
{
public:
    explicit PlayerController(GameObject &associated);

    void Start() override;
    void Update(float dt) override;
    void Render() override;
};

#endif