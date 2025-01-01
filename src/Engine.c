#include <Engine.h>

void apply_gravitational_force(Ball* ball, size_t index, float delta_time)
{
    ball->x += (ball->applied_force.x / ball->width) * delta_time;
    ball->y += (ball->applied_force.y / ball->height) * delta_time;
    if (ball->x >= WINDOW_WIDTH)
    {
        ball->x = WINDOW_WIDTH - ball->width;
        ball->applied_force.x = 0;
    }
    if (ball->x <= 0)
    {
        ball->x = 0;
        ball->applied_force.x = 0;
    }
    if (ball->y >= WINDOW_HEIGHT)
    {
        ball->y = WINDOW_HEIGHT - ball->height;
        ball->applied_force.y = 0;
    }
    if (ball->y <= 0)
    {
        ball->y = 0;
        ball->applied_force.y = 0;
    }
    // printf("[APPLY] Ball index: %ld, X force: %f Y Force: %f\n", index, ball->applied_force.x,
    //    ball->applied_force.y);
}

bool check_balls_collide(Ball ball_1, Ball ball_2)
{
    return !(ball_1.x + ball_1.width < ball_2.x || ball_2.x + ball_2.width < ball_1.x ||
             ball_1.y + ball_1.height < ball_2.y || ball_2.y + ball_2.height < ball_1.y);
}

void collision_detection(Ball* ball_input)
{
    bool bounced = false;
    if (ball_input->x + ball_input->width >= WINDOW_WIDTH || ball_input->x <= 0)
    {
        ball_input->speed_x *= -1;
        bounced = true;
    }
    if (ball_input->y <= 0 || (ball_input->y + ball_input->height >= WINDOW_HEIGHT))
    {
        ball_input->speed_y *= -1;
        bounced = true;
    }
    if (bounced && ball_count < MAX_BALL_COUNT)
    {
        add_ball_default();
    }
}

void check_gravitational_force(Ball* ball_1, Ball* ball_2, size_t index)
{
    float x_axis_force, y_axis_force = 0;
    float distance = sqrt(f_square(ball_2->x - ball_1->x) + f_square(ball_2->y - ball_1->y));
    float g_force = (GRAVITATIONAL_CONSTANT * ball_1->width * ball_2->width) / f_square(distance);
    if (!distance)
    {
        x_axis_force = 0;
        y_axis_force = 0;
    }
    else
    {
        x_axis_force = (ball_2->x - ball_1->x) / distance;
        y_axis_force = (ball_2->y - ball_1->y) / distance;
    }
    g_force *= 100;
    // printf("[CALC]Ball index: %ld X-Axis force is : %f Y-Axis force is: %f\n", index,
    // x_axis_force,
    //        y_axis_force);

    ball_1->applied_force.x += x_axis_force * g_force;
    ball_1->applied_force.y += y_axis_force * g_force;
}
