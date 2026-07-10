#ifndef INSTRUCTIONPROMPT_H
#define INSTRUCTIONPROMPT_H

#include "Component.h"
#include "Vec2.h"

class InstructionPrompt : public Component {
public:
    InstructionPrompt(GameObject& associated, float displayDuration, Vec2 screenPosition);
    
    void Start() override;
    void Update(float dt) override;
    void Render() override;

private:
    float duration;
    float elapsed;
    Vec2 screenPos;
};

#endif