#include <Render.h>

SDL_Window* p_window = NULL;
SDL_Renderer* p_renderer = NULL;
bool game_is_running = false;
int last_frame_time = 0;
float delta_time = 0;
int WINDOW_WIDTH  = 800;
int WINDOW_HEIGHT = 800;

bool initialize_window(void)
{
    if (SDL_Init(SDL_INIT_EVERYTHING) != 0)
    {
        fprintf(stderr, "Error initializing SDL. \n");
        return SDL_FALSE;
    }
    p_window = SDL_CreateWindow("Rami", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED,
                                WINDOW_WIDTH, WINDOW_HEIGHT, SDL_WINDOW_RESIZABLE);
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

void process_input()
{
    SDL_Event event;
    while (SDL_PollEvent(&event)) // Process all pending events
    {
        switch (event.type)
        {
            case SDL_QUIT:
                game_is_running = false;
                break;
            case SDL_WINDOWEVENT:
                if (event.window.event == SDL_WINDOWEVENT_RESIZED)
                {
                    WINDOW_WIDTH  = event.window.data1;
                    WINDOW_HEIGHT = event.window.data2;
                    printf("Window resized: %d x %d\n", WINDOW_WIDTH, WINDOW_HEIGHT);
                }
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