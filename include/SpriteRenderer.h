#ifndef SPRITERENDERER_H
#define SPRITERENDERER_H

#include "Component.h"
#include "Sprite.h"
#include <string>
#include "SDL_include.h"

class SpriteRenderer : public Component
{
public:
    explicit SpriteRenderer(GameObject &associated);
    SpriteRenderer(GameObject &associated, const std::string &file, int frameCountW = 1, int frameCountH = 1);

    void Open(const std::string &file);
    void SetCameraFollower(bool follow);

    void SetFrameCount(int frameCountW, int frameCountH);
    void SetFrame(int frame, SDL_RendererFlip flip = SDL_FLIP_NONE);
    int GetWidth() const;
    int GetHeight() const;
    void SetScale(float scaleX, float scaleY);
    void SetFlip(SDL_RendererFlip flip);
    void SetUseSourceFrameOffset(bool useOffset);
    void Update(float dt) override;
    void Render() override;
    void SetParallax(Vec2 factor);
    void SetRenderOffsetY(int offset);

private:
    Sprite sprite;
    Vec2 parallaxFactor;
};

#endif