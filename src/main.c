#include "main.h"

int main(int argc, char** argv)
{
    game_is_running = initialize_window();

    add_ball_default();

    // Create and Initialize a Circle (Dynamically Allocated)
    CircleObject* circle1 = malloc(sizeof(CircleObject));
    if (!circle1)
    {
        fprintf(stderr, "Failed to allocate memory for Circle1\n");
        return 1;  // Exit with error code
    }
    init_circle(circle1, "Circle1", 25, 25, 25);
    // start with randomly selected velocity
    circle1->base.velocity.x = 50.0f;
    circle1->base.velocity.y = 150.0f;

    CircleObject* circle2 = malloc(sizeof(CircleObject));
    if (!circle2)
    {
        fprintf(stderr, "Failed to allocate memory for Circle2\n");
        free(circle1);  // Free already allocated memory
        return 1;
    }
    init_circle(circle2, "Circle2", 25, 275, 25);
    circle2->base.velocity.x = 100.0f;
    circle2->base.velocity.y = -50.0f;

    CircleObject* circle3 = malloc(sizeof(CircleObject));
    if (!circle3)
    {
        fprintf(stderr, "Failed to allocate memory for Circle3\n");
        free(circle1);
        free(circle2);
        return 1;
    }
    init_circle(circle3, "Circle3", 500, 625, 25);
    circle3->base.velocity.x = -125.0f;
    circle3->base.velocity.y = -350.0f;

    // Add the Circles to the game_objects array
    add_game_object((GameObject*)circle1);
    add_game_object((GameObject*)circle2);
    add_game_object((GameObject*)circle3);

    while (game_is_running)
    {
        process_input();
        update();
        render();
    }

    destroy_window();

    return 0;
}
