#ifndef MAIN_H
#define MAIN_H

#include <Ball.h>
#include <MathHelper.h>
#include <BallHelper.h>
#include <Render.h>

#define WINDOW_WIDTH  800
#define WINDOW_HEIGHT 800
#define CENTER_X      WINDOW_WIDTH / 2
#define CENTER_Y      WINDOW_HEIGHT / 2

#define BALL_HEIGHT 40
#define BALL_WIDTH  40

#define GRAVITATIONAL_CONSTANT 1

#define MAX_BALL_COUNT 50

#define COLOR_SCALAR 500

#define DELTA_TIME_COFACTOR 1000.0f

#define FPS               30
#define FRAME_TARGET_TIME (1000 / FPS)


#endif