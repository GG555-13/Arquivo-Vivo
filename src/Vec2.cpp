#include "Vec2.h"
#include <cmath>

Vec2::Vec2() : x(0.0f), y(0.0f) {}

Vec2::Vec2(float x, float y) : x(x), y(y) {}

float Vec2::Magnitude() const
{
    return std::sqrt(x * x + y * y);
}

Vec2 Vec2::Normalized() const
{
    float mag = Magnitude();
    if (mag > 0)
    {
        return {x / mag, y / mag};
    }
    return {0, 0};
}

float Vec2::Distance(const Vec2 &other) const
{
    return (*this - other).Magnitude();
}

float Vec2::Angle() const
{
    return std::atan2(y, x);
}

Vec2 Vec2::Rotated(float angle)
{
    float cos_a = std::cos(angle);
    float sin_a = std::sin(angle);
    float new_x = x * cos_a - y * sin_a;
    float new_y = y * cos_a + x * sin_a;
    return {new_x, new_y};
}

Vec2 Vec2::operator+(const Vec2 &other) const
{
    return {x + other.x, y + other.y};
}

Vec2 Vec2::operator-(const Vec2 &other) const
{
    return {x - other.x, y - other.y};
}

Vec2 Vec2::operator*(float scalar) const
{
    return {x * scalar, y * scalar};
}