#include "Rect.h"

Rect::Rect() : x(0.0f), y(0.0f), w(0.0f), h(0.0f) {}

Rect::Rect(float x, float y, float w, float h) : x(x), y(y), w(w), h(h) {}

Vec2 Rect::Center() const
{
    return {x + w / 2.0f, y + h / 2.0f};
}

float Rect::Distance(const Rect &other) const
{
    return this->Center().Distance(other.Center());
}

bool Rect::Contains(const Vec2 &point) const
{
    return (point.x >= x && point.x <= x + w) &&
           (point.y >= y && point.y <= y + h);
}

Rect Rect::operator+(const Vec2 &vec) const
{
    return {x + vec.x, y + vec.y, w, h};
}

void Rect::SetCenter(Vec2 newCenter) {
    x = newCenter.x - w / 2.0f;
    y = newCenter.y - h / 2.0f;
}