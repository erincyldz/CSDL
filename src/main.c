#include "main.h"

int main(int argc, char** argv)
{
    game_is_running = initialize_window();

    // add_ball_default();

    // Create and Initialize a Circle (Dynamically Allocated)
    CircleObject* circle1 = malloc(sizeof(CircleObject));
    CircleObject* circle2 = malloc(sizeof(CircleObject));
    CircleObject* circle3 = malloc(sizeof(CircleObject));
    if (!circle1 || !circle2 || !circle3)
    {
        fprintf(stderr, "Failed to allocate memory for CircleObject\n");
        return 1;
    }
    init_circle(circle1, "Circle 1", 100, 100, 20);
    init_circle(circle2, "Circle 2", 200, 200, 20);
    init_circle(circle3, "Circle 3", 300, 300, 20);
    circle1->base.velocity.x = 100;
    circle2->base.velocity.x = 100;
    circle3->base.velocity.x = 100;

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
