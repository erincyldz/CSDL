#ifndef OBJECT_HELPER_H
#define OBJECT_HELPER_H
constexpr int COLOR_CHANGE_SCALAR = 500;

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

} Color_t;

typedef struct Vector2
{
    float x, y;
} Vector2_t;

}  // namespace game::object::helper
#endif
