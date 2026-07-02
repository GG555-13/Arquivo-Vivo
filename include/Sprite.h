#ifndef SPRITE_H
#define SPRITE_H

#include <string>
#include "SDL_include.h"
#include "Vec2.h"
#include <memory>

class Sprite
{
public:
    Sprite();
    explicit Sprite(const std::string &file, int frameCountW = 1, int frameCountH = 1);
    ~Sprite();

    void Open(const std::string &file);
    void Render(int x, int y, float angle = 0);
    int GetWidth() const;
    int GetHeight() const;
    bool IsOpen() const;

    void SetFrame(int frame);
    void SetFrameCount(int frameCountW, int frameCountH);
    void SetCameraFollower(bool follow);

    void SetScale(float scaleX, float scaleY);
    Vec2 GetScale() const;
    void SetFlip(SDL_RendererFlip flip);
    void SetUseSourceFrameOffset(bool useOffset);

private:
    std::shared_ptr<SDL_Texture> texture;
    int width;
    int height;
    SDL_Rect clipRect;

    int frameCountW;
    int frameCountH;
    bool cameraFollower;
    bool useSourceFrameOffset;

    Vec2 scale;
    SDL_RendererFlip flip;
};

#endif