#include "main.h"

#include <SDL2/SDL.h>
#include <math.h>
#include <stdarg.h>
#include <stdatomic.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>

SDL_Window* p_window = NULL;
SDL_Renderer* p_renderer = NULL;
bool game_is_running = false;
int last_frame_time = 0;
float delta_time = 0;
typedef enum
{
    RED,
    PINK,
    BLUE,
    CYAN,
    GREEN,
    YELLOW,
} COLOR_STATE;

typedef struct
{
    float x, y;
} Force;

struct ball
{
    float x, y;
    float width, height;
    int speed_x, speed_y;
    int r, g, b;
    COLOR_STATE c_state;
    Force applied_force;
};

// PROTOTYPES
float f_square(float num);
void add_ball(float x, float y, float width, float height, int speed_x, int speed_y);
void add_ball_default();
bool initialize_window(void);
SDL_Rect convert_to_sdl_rect(struct ball* b);
void process_input();
bool check_balls_collide(struct ball a, struct ball b);
void collision_detection(struct ball* ball_input);
void show_end_game_message_box_and_exit();
void update();
void check_gravitational_force(struct ball* a, struct ball* b, size_t index);
void render();
void destroy_window();
void setup();
void apply_gravitational_force(struct ball* a, size_t index);
//////////////////////////
struct ball* balls = NULL;
_Atomic size_t ball_count = 0;

float f_square(float num)
{
    return num * num;
}

void add_ball(float x, float y, float width, float height, int speed_x, int speed_y)
{
    // Reallocate memory to add a new ball
    size_t current_count = atomic_fetch_add(&ball_count, 1);
    struct ball* new_balls = realloc(balls, (current_count + 1) * sizeof(struct ball));
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
        (struct ball){x, y, width, height, speed_x, speed_y, r, g, b, c_state, init_force};
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
    struct ball* new_balls = realloc(balls, (current_count + 1) * sizeof(struct ball));
    if (!new_balls)
    {
        printf("Failed to allocate memory for new ball");
        exit(1);
    }
    balls = new_balls;

    // Add the new ball
    balls[current_count] =
        (struct ball){x, y, width, height, speed_x, speed_y, r, g, b, color, applied_force};
}

bool initialize_window(void)
{
    if (SDL_Init(SDL_INIT_EVERYTHING) != 0)
    {
        fprintf(stderr, "Error initializing SDL. \n");
        return SDL_FALSE;
    }
    p_window = SDL_CreateWindow("Rami", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED,
                                WINDOW_WIDTH, WINDOW_HEIGHT, SDL_WINDOW_BORDERLESS);
    if (!p_window)
    {
        fprintf(stderr, "Error creating a SDL window\n");
        return SDL_FALSE;
    }

    p_renderer = SDL_CreateRenderer(p_window, -1, 0);

    if (!p_renderer)
    {
        fprintf(stderr, "Error creating SDL renderer\n");
        return SDL_FALSE;
    }

    return SDL_TRUE;
}

SDL_Rect convert_to_sdl_rect(struct ball* b)
{
    SDL_Rect rect;
    rect.x = (int)b->x;  // Cast float to int
    rect.y = (int)b->y;
    rect.w = (int)b->width;
    rect.h = (int)b->height;
    return rect;
}

void process_input()
{
    SDL_Event event;
    SDL_PollEvent(&event);

    switch (event.type)
    {
        case SDL_QUIT:
            game_is_running = false;
            break;
        case SDL_KEYDOWN:
            if (event.key.keysym.sym == SDLK_ESCAPE)
            {
                game_is_running = false;
            }
            else if (event.key.keysym.sym == SDLK_q)
            {
                add_ball_default();
            }
            break;
    }
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

void show_end_game_message_box_and_exit()
{
    // Define the button data
    SDL_MessageBoxButtonData buttons[] = {
        {0, 0, "OK"}  // Button ID 0, labeled "OK"
    };

    // Define the message box data
    SDL_MessageBoxData message_box_data = {
        SDL_MESSAGEBOX_INFORMATION,                        // Message box type
        NULL,                                              // No parent window
        "Game Over",                                       // Title of the message box
        "You have completed the game! Press OK to exit.",  // Message text
        SDL_arraysize(buttons),                            // Number of buttons
        buttons,                                           // Buttons array
        NULL                                               // No custom colors
    };

    int button_id;
    // Show the message box
    if (SDL_ShowMessageBox(&message_box_data, &button_id) < 0)
    {
        fprintf(stderr, "Error displaying message box: %s\n", SDL_GetError());
        return;
    }

    // Check which button was pressed
    if (button_id == 0)
    {
        free(balls);  // "OK" button
        SDL_Quit();   // Clean up SDL
        exit(0);      // Exit the application
    }
}

void update()
{

    int time_to_wait = FRAME_TARGET_TIME - (SDL_GetTicks() - last_frame_time);

    if (time_to_wait > 0 && time_to_wait <= FRAME_TARGET_TIME)
    {
        SDL_Delay(time_to_wait);
    }

    delta_time = (SDL_GetTicks() - last_frame_time) / DELTA_TIME_COFACTOR;

    last_frame_time = SDL_GetTicks();
    for (size_t i = 0; i < atomic_load(&ball_count); i++)
    {

        collision_detection(&balls[i]);
        for (size_t j = 0; j < atomic_load(&ball_count); j++)
        {

            if (i == j)
                continue;
            check_gravitational_force(&balls[i], &balls[j], i);
            if (check_balls_collide(balls[i], balls[j]))
            {
                if (atomic_load(&ball_count) == 2)
                {
                    show_end_game_message_box_and_exit();
                }
                for (size_t k = j; k < atomic_load(&ball_count) - 1; ++k)
                {
                    balls[k] = balls[k + 1];
                }
                --ball_count;

                for (size_t k = i; k < atomic_load(&ball_count) - 1; ++k)
                {
                    balls[k] = balls[k + 1];
                }
                --ball_count;
                balls = realloc(balls, atomic_load(&ball_count) * sizeof(struct ball));
            }
        }
        apply_gravitational_force(&balls[i], i);
    }

    for (size_t i = 0; i < atomic_load(&ball_count); i++)
    {
        balls[i].x += balls[i].speed_x * delta_time;
        balls[i].y += balls[i].speed_y * delta_time;
        switch (balls[i].c_state)
        {
            case RED:
            {
                balls[i].b += COLOR_SCALAR * delta_time;
                if (balls[i].b == 255)
                {
                    balls[i].c_state = PINK;
                    break;
                }
                if (balls[i].b >= 255)
                {
                    balls[i].b = 255;
                    balls[i].c_state = PINK;
                    break;
                }
                break;
            }
            case PINK:
            {
                balls[i].r -= COLOR_SCALAR * delta_time;
                if (balls[i].r == 0)
                {
                    balls[i].c_state = BLUE;
                    break;
                }
                if (balls[i].r <= 0)
                {
                    balls[i].r = 0;
                    balls[i].c_state = BLUE;
                    break;
                }
                break;
            }
            case BLUE:
            {
                balls[i].g += COLOR_SCALAR * delta_time;
                if (balls[i].g == 255)
                {
                    balls[i].c_state = CYAN;
                    break;
                }
                if (balls[i].g >= 255)
                {
                    balls[i].g = 255;
                    balls[i].c_state = CYAN;
                    break;
                }
                break;
            }
            case CYAN:
            {
                balls[i].b -= COLOR_SCALAR * delta_time;
                if (balls[i].b == 0)
                {
                    balls[i].c_state = GREEN;
                    break;
                }
                if (balls[i].b <= 0)
                {
                    balls[i].b = 0;
                    balls[i].c_state = GREEN;
                    break;
                }
                break;
            }
            case GREEN:
            {
                balls[i].r += COLOR_SCALAR * delta_time;
                if (balls[i].r == 255)
                {
                    balls[i].c_state = YELLOW;
                    break;
                }
                if (balls[i].r >= 255)
                {
                    balls[i].r = 255;
                    balls[i].c_state = YELLOW;
                    break;
                }
                break;
            }
            case YELLOW:
            {
                balls[i].g -= COLOR_SCALAR * delta_time;
                if (balls[i].g == 0)
                {
                    balls[i].c_state = RED;
                    break;
                }
                if (balls[i].g <= 0)
                {
                    balls[i].g = 0;
                    balls[i].c_state = RED;
                    break;
                }
                break;
            }
        }
    }
};

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

void render()
{

    SDL_SetRenderDrawColor(p_renderer, 0, 0, 0, 255);

    SDL_RenderClear(p_renderer);

    for (size_t i = 0; i < atomic_load(&ball_count); i++)
    {
        SDL_SetRenderDrawColor(p_renderer, balls[i].r, balls[i].g, balls[i].b, 255);

        SDL_Rect ball_rect = convert_to_sdl_rect(&balls[i]);

        SDL_RenderFillRect(p_renderer, &ball_rect);
    }
    SDL_RenderPresent(p_renderer);
};

void destroy_window()
{
    SDL_DestroyRenderer(p_renderer);
    SDL_DestroyWindow(p_window);
    SDL_Quit();
};

void setup()
{
    add_ball_default();
};

int main(int argc, char** argv)
{
    game_is_running = initialize_window();

    setup();
    while (game_is_running)
    {
        process_input();
        update();
        render();
    }

    destroy_window();

    return 0;
}
