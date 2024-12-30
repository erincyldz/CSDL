#ifndef BALL_H
#define BALL_H

typedef enum
{
    RED,
    PINK,
    BLUE,
    CYAN,
    GREEN,
    YELLOW,
} COLOR_STATE;

typedef struct
{
    float x, y;
} Force;

struct ball
{
    float x, y;
    float width, height;
    int speed_x, speed_y;
    int r, g, b;
    COLOR_STATE c_state;
    Force applied_force;
};

#endif // BALL_H