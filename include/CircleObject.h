#ifndef CIRCLE_OBJECT_H
#define CIRCLE_OBJECT_H

#include <GameObject.h>

#define MAX_RADIUS 100
#define MIN_RADIUS 10

typedef struct
{
    GameObject base;  // Embed GameObject
    float radius;     // Unique property for Circle
} CircleObject;

void init_circle(CircleObject* circle, const char* name, float x, float y, float radius);
void draw_circle(void* self, SDL_Renderer* renderer);
void update_circle(void* self, float delta_time);
void destroy_circle(void* self);

#endif  // CIRCLE_OBJECT_H
