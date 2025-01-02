#ifndef GAME_OBJECT_H
#define GAME_OBJECT_H

#include <GameObjectHelper.h>
#include <SDL2/SDL.h>

#define MAX_GAME_OBJECTS 10

typedef enum
{
    CIRCLE,
    RECTANGLE
} GameObjectType;

typedef struct
{
    float x, y;
} Vector2;

// Base Object Struct
typedef struct GameObject
{
    Vector2 position;
    Vector2 velocity;
    float mass;
    GameObjectType type;
    OBJECT_COLOR_STATE color_state;
    Color color;
    void (*update)(void* self, float delta_time);
    int (*collides_with)(struct GameObject* self, struct GameObject* other);
    void (*destroy)(void* self);
    void (*update_color)(void* self, float delta_time);
} GameObject;

// Declare an array to store pointers to GameObjects
extern GameObject* game_objects[MAX_GAME_OBJECTS];
extern int game_object_count;

void add_game_object(GameObject* object);
void update_color(void* self, float delta_time);

#endif  // GAME_OBJECT_H