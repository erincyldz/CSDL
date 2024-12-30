#include <Engine.h>

void apply_gravitational_force(struct ball* a, size_t index)
{
    a->x += a->applied_force.x / a->width;
    a->y += a->applied_force.y / a->height;
    if (a->x >= WINDOW_WIDTH)
    {
        a->x = WINDOW_WIDTH - a->width;
        a->applied_force.x = 0;
    }
    if (a->x <= 0)
    {
        a->x = 0;
        a->applied_force.x = 0;
    }
    if (a->y >= WINDOW_HEIGHT)
    {
        a->y = WINDOW_HEIGHT - a->height;
        a->applied_force.y = 0;
    }
    if (a->y <= 0)
    {
        a->y = 0;
        a->applied_force.y = 0;
    }
    printf("[APPLY] Ball index: %ld, X force: %f Y Force: %f\n", index, a->applied_force.x,
           a->applied_force.y);
}

bool check_balls_collide(struct ball a, struct ball b)
{
    return !(a.x + a.width < b.x || b.x + b.width < a.x || a.y + a.height < b.y ||
             b.y + b.height < a.y);
}

void collision_detection(struct ball* ball_input)
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

void check_gravitational_force(struct ball* a, struct ball* b, size_t index)
{
    float x_axis_force, y_axis_force = 0;
    float distance = sqrt(f_square(b->x - a->x) + f_square(b->y - a->y));
    float g_force = (GRAVITATIONAL_CONSTANT * a->width * b->width) / f_square(distance);
    if (!distance)
    {
        x_axis_force = 0;
        y_axis_force = 0;
    }
    else
    {
        x_axis_force = (b->x - a->x) / distance;
        y_axis_force = (b->y - a->y) / distance;
    }
    g_force *= 100;
    printf("[CALC]Ball index: %ld X-Axis force is : %f Y-Axis force is: %f\n", index, x_axis_force,
           y_axis_force);

    a->applied_force.x += x_axis_force * g_force;
    a->applied_force.y += y_axis_force * g_force;
}
