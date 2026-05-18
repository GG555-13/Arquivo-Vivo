#include "Collider.h"
#include "GameObject.h"
#include <cmath>

#ifdef DEBUG
#include "Camera.h"
#include "Game.h"
#include <SDL2/SDL.h>
#endif

Collider::Collider(GameObject &associated, Vec2 scale, Vec2 offset) : Component(associated), scale(scale), offset(offset)
{
}

void Collider::Update(float dt)
{
    box = associated.box;

    box.w *= scale.x;
    box.h *= scale.y;

    float angleRad = associated.angleDeg * (M_PI / 180.0);

    Vec2 rotatedOffset = offset.Rotated(angleRad);

    box.SetCenter(associated.box.Center() + rotatedOffset);
}

void Collider::Render()
{
#ifdef DEBUG
    Vec2 center(box.Center());
    SDL_Point points[5];

    float angleRad = associated.angleDeg * (M_PI / 180.0);

    Vec2 point = (Vec2(box.x, box.y) - center).Rotated(angleRad) + center - Camera::pos;
    points[0] = {(int)point.x, (int)point.y};
    points[4] = {(int)point.x, (int)point.y};

    point = (Vec2(box.x + box.w, box.y) - center).Rotated(angleRad) + center - Camera::pos;
    points[1] = {(int)point.x, (int)point.y};

    point = (Vec2(box.x + box.w, box.y + box.h) - center).Rotated(angleRad) + center - Camera::pos;
    points[2] = {(int)point.x, (int)point.y};

    point = (Vec2(box.x, box.y + box.h) - center).Rotated(angleRad) + center - Camera::pos;
    points[3] = {(int)point.x, (int)point.y};

    SDL_SetRenderDrawColor(Game::GetInstance().GetRenderer(), 255, 0, 0, SDL_ALPHA_OPAQUE);
    SDL_RenderDrawLines(Game::GetInstance().GetRenderer(), points, 5);
#endif
}

void Collider::SetScale(Vec2 scale)
{
    this->scale = scale;
}

void Collider::SetOffset(Vec2 offset)
{
    this->offset = offset;
}