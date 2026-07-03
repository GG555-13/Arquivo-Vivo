#ifndef SCREENFADE_H
#define SCREENFADE_H

#include "SDL_include.h"
#include "FadeTypes.h"
#include <functional>

enum class FadeDirection { None, In, Out };

class ScreenFade {
public:
    ScreenFade();

    void FadeOut(float duration, FadeColor color, std::function<void()> onComplete);
    void FadeIn(float duration, FadeColor color);

    void Update(float dt);
    void Render();

    bool IsActive() const;
    bool IsFadingIn() const;
    bool IsFadingOut() const;
    void Reset();

private:
    float alpha = 0.0f;
    float target = 0.0f;
    float speed = 0.0f;
    FadeColor color = FadeColor::Black;
    FadeDirection direction = FadeDirection::None;

    std::function<void()> callback;
    bool hasCallback = false;

    void FireCallback();
};

#endif
