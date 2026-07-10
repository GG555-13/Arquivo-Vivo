#include "ColoredRect.h"

#include "Camera.h"
#include "Game.h"
#include "GameObject.h"

ColoredRect::ColoredRect(GameObject &associated, SDL_Color color)
    : Component(associated),
      color(color)
{
}

void ColoredRect::Update(float dt)
{
    (void)dt;
}

void ColoredRect::Render()
{
    SDL_Renderer *renderer = Game::GetInstance().GetRenderer();
    SDL_SetRenderDrawBlendMode(renderer, SDL_BLENDMODE_BLEND);
    SDL_SetRenderDrawColor(renderer, color.r, color.g, color.b, color.a);
    SDL_Rect rect = {
        static_cast<int>(associated.box.x - Camera::pos.x),
        static_cast<int>(associated.box.y - Camera::pos.y),
        static_cast<int>(associated.box.w),
        static_cast<int>(associated.box.h)
    };
    SDL_RenderFillRect(renderer, &rect);
}

void ColoredRect::SetColor(SDL_Color value)
{
    color = value;
}
