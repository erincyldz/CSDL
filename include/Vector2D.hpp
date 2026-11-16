#pragma once
#include <cmath>
#include <ostream>
#include <stdexcept>

namespace game::math
{
constexpr double PI = 3.14159265358979323846;

class Vector2D
{
  public:
    constexpr Vector2D() = default;
    constexpr Vector2D(double x, double y) : m_x(x), m_y(y) {}

    constexpr double getX() const
    {
        return m_x;
    }
    constexpr double getY() const
    {
        return m_y;
    }
    void setX(double x)
    {
        m_x = x;
    }
    void setY(double y)
    {
        m_y = y;
    }

    constexpr double lengthSquared() const
    {
        return m_x * m_x + m_y * m_y;
    }
    double magnitude() const
    {
        return std::sqrt(lengthSquared());
    }
    Vector2D normalized() const
    {
        const double mag = magnitude();
        return (mag > 0.0) ? Vector2D(m_x / mag, m_y / mag) : Vector2D(0.0, 0.0);
    }
    constexpr double dot(const Vector2D& other) const
    {
        return m_x * other.m_x + m_y * other.m_y;
    }
    // Scalar z-component of the 3D cross product.
    constexpr double cross(const Vector2D& other) const
    {
        return m_x * other.m_y - m_y * other.m_x;
    }

    constexpr Vector2D operator+(const Vector2D& o) const
    {
        return {m_x + o.m_x, m_y + o.m_y};
    }
    constexpr Vector2D operator-(const Vector2D& o) const
    {
        return {m_x - o.m_x, m_y - o.m_y};
    }
    constexpr Vector2D operator-() const
    {
        return {-m_x, -m_y};
    }
    constexpr Vector2D operator*(double s) const
    {
        return {m_x * s, m_y * s};
    }
    Vector2D operator/(double s) const
    {
        if (s == 0.0)
            throw std::invalid_argument("Vector2D: division by zero");
        return {m_x / s, m_y / s};
    }
    Vector2D& operator+=(const Vector2D& o)
    {
        m_x += o.m_x;
        m_y += o.m_y;
        return *this;
    }
    Vector2D& operator-=(const Vector2D& o)
    {
        m_x -= o.m_x;
        m_y -= o.m_y;
        return *this;
    }
    Vector2D& operator*=(double s)
    {
        m_x *= s;
        m_y *= s;
        return *this;
    }
    Vector2D& operator/=(double s)
    {
        *this = *this / s;
        return *this;
    }
    constexpr bool operator==(const Vector2D& o) const
    {
        return m_x == o.m_x && m_y == o.m_y;
    }
    constexpr bool operator!=(const Vector2D& o) const
    {
        return !(*this == o);
    }

    friend constexpr Vector2D operator*(double s, const Vector2D& v)
    {
        return v * s;
    }
    friend std::ostream& operator<<(std::ostream& os, const Vector2D& v)
    {
        return os << "(" << v.m_x << ", " << v.m_y << ")";
    }

  private:
    double m_x = 0.0;
    double m_y = 0.0;
};
}  // namespace game::math
