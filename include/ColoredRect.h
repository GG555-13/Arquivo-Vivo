#ifndef COLOREDRECT_H
#define COLOREDRECT_H

#include "Component.h"
#include "SDL_include.h"

class ColoredRect : public Component
{
public:
    explicit ColoredRect(GameObject &associated, SDL_Color color);

    void Update(float dt) override;
    void Render() override;
    void SetColor(SDL_Color value);

private:
    SDL_Color color;
};

#endif
