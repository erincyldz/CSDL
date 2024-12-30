#ifndef BALL_HELPER_H
#define BALL_HELPER_H
#include <Ball.h>
#include <stdatomic.h>
#include <stddef.h>  // Include this for size_t
#include <stdio.h>
#include <stdlib.h>

// Declare the variables as extern
extern _Atomic size_t ball_count;
extern struct ball* balls;

void add_ball(float x, float y, float width, float height, int speed_x, int speed_y);
void add_ball_default();
#endif  // BALL_HELPER_H