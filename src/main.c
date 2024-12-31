#include "main.h"

int main(int argc, char** argv)
{
    game_is_running = initialize_window();

    add_ball_default();

    // Create and Initialize a Circle
    CircleObject circle1;
    init_circle(&circle1, "MyCircle", 25, 25, 25);

    CircleObject circle2;
    init_circle(&circle2, "MyCircle", 25, 275, 25);
    circle2.base.velocity.y = -50.0f;
    // Add the Circle to the game_objects array
    add_game_object((GameObject*)&circle1);
    add_game_object((GameObject*)&circle2);

    while (game_is_running)
    {
        process_input();
        update();
        render();
    }

    destroy_window();

    return 0;
}
