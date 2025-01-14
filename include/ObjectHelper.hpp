#ifndef OBJECT_HELPER_H
#define OBJECT_HELPER_H
constexpr int COLOR_CHANGE_SCALAR = 500;
constexpr float FRICTION_COEFFICIENT = 0.00f;
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

}  // namespace game::object::helper
#endif
