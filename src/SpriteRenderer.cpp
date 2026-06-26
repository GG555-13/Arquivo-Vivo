#include "SpriteRenderer.h"
#include "GameObject.h"
#include "Camera.h"

SpriteRenderer::SpriteRenderer(GameObject &associated) : Component(associated)
{
}

SpriteRenderer::SpriteRenderer(GameObject &associated, const std::string &file, int frameCountW, int frameCountH)
    : Component(associated), sprite(file, frameCountW, frameCountH)
{
    parallaxFactor = Vec2(1.0f, 1.0f);
    associated.box.w = sprite.GetWidth();
    associated.box.h = sprite.GetHeight();
}


void SpriteRenderer::Open(const std::string &file)
{
    sprite.Open(file);
    associated.box.w = sprite.GetWidth();
    associated.box.h = sprite.GetHeight();
}

void SpriteRenderer::SetFrameCount(int frameCountW, int frameCountH)
{
    sprite.SetFrameCount(frameCountW, frameCountH);
}

void SpriteRenderer::SetCameraFollower(bool follow) {
    sprite.SetCameraFollower(follow);
}

int SpriteRenderer::GetWidth() const {
    return sprite.GetWidth();
}

int SpriteRenderer::GetHeight() const {
    return sprite.GetHeight();
}

void SpriteRenderer::SetFrame(int frame, SDL_RendererFlip flip) {
    sprite.SetFrame(frame);
    sprite.SetFlip(flip); 
}

void SpriteRenderer::SetScale(float scaleX, float scaleY) {
    Vec2 oldCenter = associated.box.Center();
    sprite.SetScale(scaleX, scaleY);
    associated.box.w = sprite.GetWidth();
    associated.box.h = sprite.GetHeight();
    associated.box.SetCenter(oldCenter);
}

void SpriteRenderer::SetFlip(SDL_RendererFlip flip) {
    sprite.SetFlip(flip); 
}

void SpriteRenderer::SetUseSourceFrameOffset(bool useOffset) {
    sprite.SetUseSourceFrameOffset(useOffset);
}

void SpriteRenderer::SetParallax(Vec2 factor)
{
    parallaxFactor = factor;
}

void SpriteRenderer::Update(float /*dt*/) {}

void SpriteRenderer::Render()
{
    float targetX = associated.box.x - (Camera::pos.x * parallaxFactor.x);
    float targetY = associated.box.y - (Camera::pos.y * parallaxFactor.y);
    
    float finalX = targetX + Camera::pos.x;
    float finalY = targetY + Camera::pos.y;

    sprite.Render(finalX, finalY); 
}
