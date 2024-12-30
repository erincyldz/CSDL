#ifndef ENGINE_H
#define ENGINE_H

#include <Ball.h>
#include <BallHelper.h>
#include <MathHelper.h>
#include <main.h>
#include <math.h>
#include <stdbool.h>

void apply_gravitational_force(struct ball* a, size_t index);
bool check_balls_collide(struct ball a, struct ball b);
void collision_detection(struct ball* ball_input);
void check_gravitational_force(struct ball* a, struct ball* b, size_t index);

#endif  // ENGINE_H