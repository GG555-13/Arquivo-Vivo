#include "ScreenFade.h"
#include "Game.h"
#include <algorithm>
#include <cmath>

ScreenFade::ScreenFade() {}

void ScreenFade::FadeOut(float duration, FadeColor fadeColor, std::function<void()> onComplete) {
    alpha = 0.0f;
    target = 1.0f;
    speed = (duration > 0.0f) ? (1.0f / duration) : 9999.0f;
    color = fadeColor;
    direction = FadeDirection::Out;
    callback = std::move(onComplete);
    hasCallback = true;
}

void ScreenFade::FadeIn(float duration, FadeColor fadeColor) {
    alpha = 1.0f;
    target = 0.0f;
    speed = (duration > 0.0f) ? (1.0f / duration) : 9999.0f;
    color = fadeColor;
    direction = FadeDirection::In;
    hasCallback = false;
}

void ScreenFade::Update(float dt) {
    if (!IsActive()) return;

    if (alpha < target)
        alpha = std::min(alpha + speed * dt, target);
    else if (alpha > target)
        alpha = std::max(alpha - speed * dt, target);

    if (!IsActive() && hasCallback) {
        FireCallback();
    }
}

void ScreenFade::Render() {
    if (alpha <= 0.001f) return;

    auto* r = Game::GetInstance().GetRenderer();
    SDL_SetRenderDrawBlendMode(r, SDL_BLENDMODE_BLEND);

    Uint8 c = (Uint8)(alpha * 255);
    if (color == FadeColor::White)
        SDL_SetRenderDrawColor(r, 255, 255, 255, c);
    else
        SDL_SetRenderDrawColor(r, 0, 0, 0, c);

    SDL_Rect full = {0, 0, Game::GetInstance().GetWidth(), Game::GetInstance().GetHeight()};
    SDL_RenderFillRect(r, &full);
}

bool ScreenFade::IsActive() const {
    return std::abs(alpha - target) > 0.001f;
}

bool ScreenFade::IsFadingIn() const {
    return IsActive() && direction == FadeDirection::In;
}

bool ScreenFade::IsFadingOut() const {
    return IsActive() && direction == FadeDirection::Out;
}

void ScreenFade::Reset() {
    alpha = 0.0f;
    target = 0.0f;
    speed = 0.0f;
    direction = FadeDirection::None;
    hasCallback = false;
    callback = nullptr;
}

void ScreenFade::FireCallback() {
    hasCallback = false;
    auto cb = std::move(callback);
    callback = nullptr;
    if (cb) cb();
}
