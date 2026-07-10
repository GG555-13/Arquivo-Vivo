#ifndef BLINKINGCARET_H
#define BLINKINGCARET_H

#include "Component.h"
#include "SDL_include.h"

class BlinkingCaret : public Component
{
public:
    explicit BlinkingCaret(GameObject &associated,
                           SDL_Color color = {255, 255, 255, 255},
                           float blinkInterval = 0.45f);

    void Update(float dt) override;
    void Render() override;
    void SetEnabled(bool value);

private:
    SDL_Color color;
    float blinkInterval;
    float elapsed;
    bool enabled;
};

#endif
