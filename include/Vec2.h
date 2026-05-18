#ifndef VEC2_H
#define VEC2_H

struct Vec2
{
    float x, y;

    Vec2();
    Vec2(float x, float y);

    float Magnitude() const;
    Vec2 Normalized() const;
    float Distance(const Vec2 &other) const;
    float Angle() const;
    Vec2 Rotated(float angle);

    Vec2 operator+(const Vec2 &other) const;
    Vec2 operator-(const Vec2 &other) const;
    Vec2 operator*(float scalar) const;
};

#endif