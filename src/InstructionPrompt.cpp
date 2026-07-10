#include "InstructionPrompt.h"
#include "Camera.h"
#include "GameObject.h"

InstructionPrompt::InstructionPrompt(GameObject& associated, float displayDuration, Vec2 screenPosition)
    : Component(associated), duration(displayDuration), elapsed(0.0f), screenPos(screenPosition) {}

void InstructionPrompt::Start() {}

void InstructionPrompt::Update(float dt) {
    elapsed += dt;

    associated.box.SetCenter(Vec2(Camera::pos.x + screenPos.x, Camera::pos.y + screenPos.y));

    if (elapsed >= duration) {
        associated.RequestDelete();
    }
}

void InstructionPrompt::Render() {}