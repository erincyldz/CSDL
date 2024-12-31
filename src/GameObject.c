#include <GameObject.h>

GameObject* game_objects[MAX_GAME_OBJECTS] = {NULL};  // Correct: array of pointers
int game_object_count = 0;                            // Initialize the count of game objects

void add_game_object(GameObject* object)
{
    if (game_object_count < MAX_GAME_OBJECTS)
    {
        game_objects[game_object_count++] = object;
    }
    else
    {
        fprintf(stderr, "Max game objects reached\n");
    }
}
