#ifndef RECT_OBJECT_H
#define RECT_OBJECT_H

#include <GameObject.h>
#include <GameObjectHelper.h>

#define MAX_SIZE 100
#define MIN_SIZE 10

typedef struct RectObject
{
    GameObject base;      // Inherits from GameObject
    float width, height;  // RECT-specific property
} RectObject;

void init_rect(RectObject* RECT, float x, float y, float width, float height);
// void draw_rect(void* self, SDL_Renderer* renderer);
void update_rect(void* self, float delta_time);
void destroy_rect(void* self);

#endif  // RECT_OBJECT_H
