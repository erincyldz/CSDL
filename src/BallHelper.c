#include <BallHelper.h>

// Define the variables here
_Atomic size_t ball_count = 0;
Ball* balls = NULL;

void add_ball(float x, float y, float width, float height, int speed_x, int speed_y)
{
    // Reallocate memory to add a new ball
    size_t current_count = atomic_fetch_add(&ball_count, 1);
    Ball* new_balls = realloc(balls, (current_count + 1) * sizeof(Ball));
    if (!new_balls)
    {
        perror("Failed to allocate memory for new ball");
        exit(1);
    }
    balls = new_balls;
    int r, g, b;
    r = 0;
    g = 255;
    b = 0;
    COLOR_STATE c_state = GREEN;
    Force init_force = {0, 0};  // Add the new ball
    balls[current_count] =
        (Ball){x, y, width, height, speed_x, speed_y, r, g, b, c_state, init_force};
}

void add_ball_default()
{
    float speed_x, speed_y;
    int x = rand() % 800;
    int y = rand() % 600;
    if (x >= 800 - 15)
    {
        x = 800 - 15;
    }
    if (y >= 600 - 15)
    {
        y = 600 - 15;
    }
    float width = rand() % 30;
    float height = width;
    int direction = rand() % 2;
    if (direction == 0)
    {
        speed_x = -100;
        speed_y = -100;
    }
    else
    {
        speed_x = 100;
        speed_y = 100;
    }
    // speed_x = 0;
    // speed_y = 0;
    Force applied_force = {0, 0};
    int r = 255;
    int g = 0;
    int b = 0;
    size_t current_count = atomic_fetch_add(&ball_count, 1);
    COLOR_STATE color = RED;
    // Reallocate memory to add a new ball
    Ball* new_balls = realloc(balls, (current_count + 1) * sizeof(Ball));
    if (!new_balls)
    {
        printf("Failed to allocate memory for new ball");
        exit(1);
    }
    balls = new_balls;

    // Add the new ball
    balls[current_count] =
        (Ball){x, y, width, height, speed_x, speed_y, r, g, b, color, applied_force};
}
