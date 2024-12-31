#ifndef GAME_OBJECT_H
#define GAME_OBJECT_H

#include <GameObjectHelper.h>
#include <SDL2/SDL.h>

#define MAX_GAME_OBJECTS 5

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
    char name[50];
    Vector2 position;
    Vector2 velocity;
    // Force applied_force;
    float mass;
    GameObjectType type;                               // Type of the object
    void (*draw)(void* self, SDL_Renderer* renderer);  // Function pointer for drawing
    void (*update)(void* self, float delta_time);      // Function pointer for updating
    int (*collides_with)(struct GameObject* self, struct GameObject* other);
    void (*destroy)(void* self);
} GameObject;

// Declare an array to store pointers to GameObjects
extern GameObject* game_objects[MAX_GAME_OBJECTS];
extern int game_object_count;

void add_game_object(GameObject* object);

#endif  // GAME_OBJECT_H