#ifndef BALL_H
#define BALL_H
#include <GameObjectHelper.h>

typedef struct
{
    float x, y;
    float width, height;
    int speed_x, speed_y;
    int r, g, b;
    OBJECT_COLOR_STATE c_state;
    Force applied_force;
} Ball;

#endif  // BALL_H