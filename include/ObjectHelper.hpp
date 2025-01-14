#ifndef OBJECT_HELPER_H
#define OBJECT_HELPER_H
#include <cmath>
#include <iostream>
constexpr int COLOR_CHANGE_SCALAR = 500;
constexpr float FRICTION_COEFFICIENT = 0.0f;
constexpr double GRAVITATIONAL_CONSTANT = 100;
namespace game::object::helper
{
enum class ObjectColor
{
    RED,
    PINK,
    BLUE,
    CYAN,
    GREEN,
    YELLOW
};

enum class ObjectType
{
    RECTANGLE = 0,
    CIRCLE
};

typedef struct Color
{
    int r, g, b;
    int a;

} Color_t;

typedef struct Vector2
{
    double x, y;
} Vector2_t;

class Vector2D
{
  private:
    double x;
    double y;

  public:
    // Constructors
    Vector2D() : x(0), y(0) {}
    Vector2D(double x, double y) : x(x), y(y) {}

    // Getters and Setters
    double getX() const
    {
        return x;
    }
    double getY() const
    {
        return y;
    }
    void setX(double x)
    {
        this->x = x;
    }
    void setY(double y)
    {
        this->y = y;
    }
    void reverseX()
    {
        this->x *= -1;
    }
    void reverseY()
    {
        this->y *= -1;
    }
    // Magnitude (length) of the vector
    double magnitude() const
    {
        return std::sqrt(x * x + y * y);
    }

    // Normalize the vector
    Vector2D normalized() const
    {
        double mag = magnitude();
        return (mag > 0) ? Vector2D(x / mag, y / mag) : Vector2D(0, 0);
    }

    // Dot product
    double dot(const Vector2D& other) const
    {
        return x * other.x + y * other.y;
    }

    // Cross product (returns the scalar result for 2D)
    double cross(const Vector2D& other) const
    {
        return x * other.y - y * other.x;
    }

    // Operator Overloads

    // Addition
    Vector2D operator+(const Vector2D& other) const
    {
        return Vector2D(x + other.x, y + other.y);
    }

    Vector2D& operator+=(const Vector2D& other)
    {
        x += other.x;
        y += other.y;
        return *this;
    }

    // Subtraction
    Vector2D operator-(const Vector2D& other) const
    {
        return Vector2D(x - other.x, y - other.y);
    }

    Vector2D& operator-=(const Vector2D& other)
    {
        x -= other.x;
        y -= other.y;
        return *this;
    }

    // Scalar Multiplication
    Vector2D operator*(double scalar) const
    {
        return Vector2D(x * scalar, y * scalar);
    }

    Vector2D& operator*=(double scalar)
    {
        x *= scalar;
        y *= scalar;
        return *this;
    }

    // Scalar Division
    Vector2D operator/(double scalar) const
    {
        if (scalar == 0)
            throw std::runtime_error("Division by zero.");
        return Vector2D(x / scalar, y / scalar);
    }

    Vector2D& operator/=(double scalar)
    {
        if (scalar == 0)
            throw std::runtime_error("Division by zero.");
        x /= scalar;
        y /= scalar;
        return *this;
    }

    // Unary Negation
    Vector2D operator-() const
    {
        return Vector2D(-x, -y);
    }

    // Equality Comparison
    bool operator==(const Vector2D& other) const
    {
        return (x == other.x) && (y == other.y);
    }

    bool operator!=(const Vector2D& other) const
    {
        return !(*this == other);
    }

    // Stream Insertion (for easy printing)
    friend std::ostream& operator<<(std::ostream& os, const Vector2D& vec)
    {
        os << "(" << vec.x << ", " << vec.y << ")";
        return os;
    }
};

}  // namespace game::object::helper
#endif
