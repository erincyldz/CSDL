#ifndef ENGINE_H
#define ENGINE_H

#include <Ball.h>
#include <BallHelper.h>
#include <MathHelper.h>
#include <main.h>
#include <math.h>
#include <stdbool.h>

void apply_gravitational_force(Ball* ball, size_t index);
bool check_balls_collide(Ball ball_1, Ball ball_2);
void collision_detection(Ball* ball_input);
void check_gravitational_force(Ball* ball_1, Ball* ball_2, size_t index);

#endif  // ENGINE_H