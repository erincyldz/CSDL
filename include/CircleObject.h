#ifndef CIRCLE_OBJECT_H
#define CIRCLE_OBJECT_H

#include <GameObject.h>
#include <GameObjectHelper.h>

#define MAX_RADIUS 100
#define MIN_RADIUS 10

typedef struct CircleObject
{
    GameObject base;  // Inherits from GameObject
    float radius;     // Circle-specific property
} CircleObject;

void init_circle(CircleObject* circle, float x, float y, float radius);
void update_circle(void* self, float delta_time);
void destroy_circle(void* self);

#endif  // CIRCLE_OBJECT_H
