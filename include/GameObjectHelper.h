#ifndef OBJECT_HELPER_H
#define OBJECT_HELPER_H
#include <math.h>

#define COLOR_CHANGE_SCALAR 500

// forward declaration
typedef struct GameObject GameObject;
typedef struct CircleObject CircleObject;

typedef enum
{
    RED,
    PINK,
    BLUE,
    CYAN,
    GREEN,
    YELLOW,
} OBJECT_COLOR_STATE;

typedef struct
{
    int r, g, b;
} Color;

typedef struct
{
    float x, y;
} Force;

void border_collision_detection(void* self, float delta_time);
// // Collision detection functions
int circle_circle_collision(CircleObject* c1, CircleObject* c2);
// // int circle_rect_collision(CircleObject* circle, RectObject* rect);
// // int rect_rect_collision(RectObject* r1, RectObject* r2);
int check_collision(GameObject* obj1, GameObject* obj2);

#endif  // OBJECT_HELPER_H