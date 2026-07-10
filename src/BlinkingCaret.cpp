#include "BlinkingCaret.h"

#include "Camera.h"
#include "Game.h"
#include "GameObject.h"

BlinkingCaret::BlinkingCaret(GameObject &associated, SDL_Color color, float blinkInterval)
    : Component(associated),
      color(color),
      blinkInterval(blinkInterval),
      elapsed(0.0f),
      enabled(true)
{
}

void BlinkingCaret::Update(float dt)
{
    elapsed += dt;
    if (elapsed >= blinkInterval * 2.0f)
    {
        elapsed = 0.0f;
    }
}

void BlinkingCaret::Render()
{
    if (!enabled || elapsed >= blinkInterval)
    {
        return;
    }

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

void BlinkingCaret::SetEnabled(bool value)
{
    enabled = value;
}
