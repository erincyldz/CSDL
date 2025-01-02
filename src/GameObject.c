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

void update_color(void* self, float delta_time)
{
    GameObject* game_object = (GameObject*)self;  // Cast to game_object
    switch (game_object->color_state)
    {
        case RED:
            game_object->color.b += COLOR_CHANGE_SCALAR * delta_time;
            if (game_object->color.b == 255)
            {
                game_object->color_state = PINK;
                break;
            }
            if (game_object->color.b >= 255)
            {
                game_object->color.b = 255;
                game_object->color_state = PINK;
                break;
            }
            break;
        case PINK:
            game_object->color.r -= COLOR_CHANGE_SCALAR * delta_time;
            if (game_object->color.r == 0)
            {
                game_object->color_state = BLUE;
                break;
            }
            if (game_object->color.r <= 0)
            {
                game_object->color.r = 0;
                game_object->color_state = BLUE;
                break;
            }
            break;
        case BLUE:
            game_object->color.g += COLOR_CHANGE_SCALAR * delta_time;
            if (game_object->color.g == 255)
            {
                game_object->color_state = CYAN;
                break;
            }
            if (game_object->color.g >= 255)
            {
                game_object->color.g = 255;
                game_object->color_state = CYAN;
                break;
            }
            break;
        case CYAN:
            game_object->color.b -= COLOR_CHANGE_SCALAR * delta_time;
            if (game_object->color.b == 0)
            {
                game_object->color_state = GREEN;
                break;
            }
            if (game_object->color.b >= 0)
            {
                game_object->color.b = 0;
                game_object->color_state = GREEN;
                break;
            }
            break;
        case GREEN:
            game_object->color.r += COLOR_CHANGE_SCALAR * delta_time;
            if (game_object->color.r == 255)
            {
                game_object->color_state = YELLOW;
                break;
            }
            if (game_object->color.r >= 255)
            {
                game_object->color.r = 255;
                game_object->color_state = YELLOW;
                break;
            }
            break;
        case YELLOW:
            game_object->color.g -= COLOR_CHANGE_SCALAR * delta_time;
            if (game_object->color.g == 0)
            {
                game_object->color_state = RED;
                break;
            }
            if (game_object->color.g <= 0)
            {
                game_object->color.g = 0;
                game_object->color_state = RED;
                break;
            }
            break;

        default:
            break;
    }
}
