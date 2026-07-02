#include "Sprite.h"
#include <SDL_include.h>
#include <iostream>
#include "Game.h"
#include "Resources.h"
#include "Camera.h"

Sprite::Sprite() : texture(nullptr), renderOffsetY(0), width(0), height(0), clipRect({0, 0, 0, 0}), frameCountW(1), frameCountH(1), cameraFollower(false), useSourceFrameOffset(true), scale(1.0f, 1.0f), flip(SDL_FLIP_NONE) {}

Sprite::Sprite(const std::string &file, int frameCountW, int frameCountH)
    : texture(nullptr), renderOffsetY(0), width(0), height(0), clipRect({0, 0, 0, 0}), cameraFollower(false), useSourceFrameOffset(true), scale(1.0f, 1.0f), flip(SDL_FLIP_NONE)
{
    this->frameCountW = frameCountW;
    this->frameCountH = frameCountH;
    Open(file);
}

Sprite::~Sprite()
{
}

void Sprite::Open(const std::string &file)
{

    texture = Resources::GetImage(file);

    if (!texture)
    {
        return;
    }

    SDL_QueryTexture(texture.get(), nullptr, nullptr, &width, &height);
    SetFrame(0);
}

void Sprite::SetCameraFollower(bool follow)
{
    cameraFollower = follow;
}

void Sprite::Render(int x, int y, float angle)
{
    if (!texture)
        return;

    SDL_Rect dstRect;
    if (cameraFollower)
    {
        dstRect.x = x;
        dstRect.y = y;
    }
    else
    {
        dstRect.x = x - Camera::pos.x;
        dstRect.y = y - Camera::pos.y;
    }

    dstRect.w = clipRect.w * scale.x;
    dstRect.h = clipRect.h * scale.y;

    if (useSourceFrameOffset)
    {
        dstRect.x += (clipRect.w - dstRect.w) / 2;
        dstRect.y += (clipRect.h - dstRect.h) / 2;
    } else {
        dstRect.y += renderOffsetY;
    }

    SDL_RenderCopyEx(Game::GetInstance().GetRenderer(), texture.get(), &clipRect, &dstRect, angle, nullptr, flip);
}

int Sprite::GetWidth() const
{
    if (frameCountW > 0)
    {
        return (width / frameCountW) * scale.x;
    }
    return 0;
}

int Sprite::GetHeight() const
{
    if (frameCountH > 0)
    {
        return (height / frameCountH) * scale.y;
    }
    return 0;
}

bool Sprite::IsOpen() const
{
    return texture != nullptr;
}

void Sprite::SetFrame(int frame)
{
    int baseFrameWidth = width / frameCountW;
    int baseFrameHeight = height / frameCountH;

    int column = frame % frameCountW;
    int row = frame / frameCountW;

    clipRect.x = column * baseFrameWidth;
    clipRect.y = row * baseFrameHeight;
    clipRect.w = baseFrameWidth;
    clipRect.h = baseFrameHeight;
}

void Sprite::SetFrameCount(int frameCountW, int frameCountH)
{
    this->frameCountW = frameCountW;
    this->frameCountH = frameCountH;
    if (texture)
    {
        SetFrame(0);
    }
}

void Sprite::SetScale(float scaleX, float scaleY)
{
    if (scaleX != 0)
    {
        scale.x = scaleX;
    }
    if (scaleY != 0)
    {
        scale.y = scaleY;
    }
}

Vec2 Sprite::GetScale() const
{
    return scale;
}

void Sprite::SetFlip(SDL_RendererFlip flipValue)
{
    flip = flipValue;
}

void Sprite::SetUseSourceFrameOffset(bool useOffset)
{
    useSourceFrameOffset = useOffset;
}

void Sprite::SetRenderOffsetY(int offset)
{
    renderOffsetY = offset;
}