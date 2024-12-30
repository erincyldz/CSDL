#include "main.h"

int main(int argc, char** argv)
{
    game_is_running = initialize_window();

    add_ball_default();
    while (game_is_running)
    {
        process_input();
        update();
        render();
    }

    destroy_window();

    return 0;
}
