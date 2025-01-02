#include "main.h"

int main(int argc, char** argv)
{
    game_is_running = initialize_window();

    // add_ball_default();

    // Create and Initialize a Circle (Dynamically Allocated)
    CircleObject* circle1 = malloc(sizeof(CircleObject));
    CircleObject* circle2 = malloc(sizeof(CircleObject));
    CircleObject* circle3 = malloc(sizeof(CircleObject));
    RectObject* rect1 = malloc(sizeof(RectObject));
    RectObject* rect2 = malloc(sizeof(RectObject));
    if (!circle1 || !circle2 || !circle3 || !rect1 || !rect2)
    {
        fprintf(stderr, "Failed to allocate memory for CircleObject\n");
        return 1;
    }
    init_circle(circle1, 100, 100, 20);
    init_circle(circle2, 200, 200, 20);
    init_circle(circle3, 300, 300, 20);
    init_rect(rect1, 400, 400, 40, 40);
    init_rect(rect2, 500, 500, 40, 40);
    circle1->base.velocity.x = 100;
    circle2->base.velocity.x = 100;
    circle3->base.velocity.x = 100;
    rect1->base.velocity.x = -100;
    rect1->base.velocity.y = -120;
    rect2->base.velocity.y = -25;
    circle1->base.color_state = RED;
    circle2->base.color_state = RED;
    circle3->base.color_state = RED;
    rect1->base.color_state = RED;
    rect2->base.color_state = RED;
    Color color = {255, 0, 0};
    circle1->base.color = color;
    circle2->base.color = color;
    circle3->base.color = color;
    rect1->base.color = color;
    rect2->base.color = color;
    // Add the Circles to the game_objects array
    add_game_object((GameObject*)circle1);
    add_game_object((GameObject*)circle2);
    add_game_object((GameObject*)circle3);
    add_game_object((GameObject*)rect1);
    add_game_object((GameObject*)rect2);
    while (game_is_running)
    {
        process_input();
        update();
        render();
    }
    destroy_window();

    return 0;
}
