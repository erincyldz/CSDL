#include <Render.h>

SDL_Window* p_window = NULL;
SDL_Renderer* p_renderer = NULL;
bool game_is_running = false;
int last_frame_time = 0;
float delta_time = 0;
int WINDOW_WIDTH = 800;
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
    while (SDL_PollEvent(&event))  // Process all pending events
    {
        switch (event.type)
        {
            case SDL_QUIT:
                game_is_running = false;
                break;
            case SDL_WINDOWEVENT:
                if (event.window.event == SDL_WINDOWEVENT_RESIZED)
                {
                    WINDOW_WIDTH = event.window.data1;
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

SDL_Rect convert_to_sdl_rect(Ball* ball)
{
    SDL_Rect rect;
    rect.x = (int)ball->x;  // Cast float to int
    rect.y = (int)ball->y;
    rect.w = (int)ball->width;
    rect.h = (int)ball->height;
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

    // call the draw function for each game_objects
    for (size_t i = 0; i < game_object_count; i++)
    {
        game_objects[i]->update(game_objects[i], delta_time);
    }

    // for (size_t i = 0; i < atomic_load(&ball_count); i++)
    // {
    //     collision_detection(&balls[i]);
    //     for (size_t j = 0; j < atomic_load(&ball_count); j++)
    //     {
    //         if (i == j)
    //             continue;
    //         check_gravitational_force(&balls[i], &balls[j], i);
    //         if (check_balls_collide(balls[i], balls[j]))
    //         {
    //             if (atomic_load(&ball_count) == 2)
    //             {
    //                 show_end_game_message_box_and_exit();
    //             }
    //             for (size_t k = j; k < atomic_load(&ball_count) - 1; ++k)
    //             {
    //                 balls[k] = balls[k + 1];
    //             }
    //             --ball_count;

    //             for (size_t k = i; k < atomic_load(&ball_count) - 1; ++k)
    //             {
    //                 balls[k] = balls[k + 1];
    //             }
    //             --ball_count;
    //             balls = realloc(balls, atomic_load(&ball_count) * sizeof(Ball));
    //         }
    //     }
    //     apply_gravitational_force(&balls[i], i, delta_time);
    // }

    // for (size_t i = 0; i < atomic_load(&ball_count); i++)
    // {
    //     balls[i].x += balls[i].speed_x * delta_time;
    //     balls[i].y += balls[i].speed_y * delta_time;
    //     switch (balls[i].c_state)
    //     {
    //         case RED:
    //         {
    //             balls[i].b += COLOR_SCALAR * delta_time;
    //             if (balls[i].b == 255)
    //             {
    //                 balls[i].c_state = PINK;
    //                 break;
    //             }
    //             if (balls[i].b >= 255)
    //             {
    //                 balls[i].b = 255;
    //                 balls[i].c_state = PINK;
    //                 break;
    //             }
    //             break;
    //         }
    //         case PINK:
    //         {
    //             balls[i].r -= COLOR_SCALAR * delta_time;
    //             if (balls[i].r == 0)
    //             {
    //                 balls[i].c_state = BLUE;
    //                 break;
    //             }
    //             if (balls[i].r <= 0)
    //             {
    //                 balls[i].r = 0;
    //                 balls[i].c_state = BLUE;
    //                 break;
    //             }
    //             break;
    //         }
    //         case BLUE:
    //         {
    //             balls[i].g += COLOR_SCALAR * delta_time;
    //             if (balls[i].g == 255)
    //             {
    //                 balls[i].c_state = CYAN;
    //                 break;
    //             }
    //             if (balls[i].g >= 255)
    //             {
    //                 balls[i].g = 255;
    //                 balls[i].c_state = CYAN;
    //                 break;
    //             }
    //             break;
    //         }
    //         case CYAN:
    //         {
    //             balls[i].b -= COLOR_SCALAR * delta_time;
    //             if (balls[i].b == 0)
    //             {
    //                 balls[i].c_state = GREEN;
    //                 break;
    //             }
    //             if (balls[i].b <= 0)
    //             {
    //                 balls[i].b = 0;
    //                 balls[i].c_state = GREEN;
    //                 break;
    //             }
    //             break;
    //         }
    //         case GREEN:
    //         {
    //             balls[i].r += COLOR_SCALAR * delta_time;
    //             if (balls[i].r == 255)
    //             {
    //                 balls[i].c_state = YELLOW;
    //                 break;
    //             }
    //             if (balls[i].r >= 255)
    //             {
    //                 balls[i].r = 255;
    //                 balls[i].c_state = YELLOW;
    //                 break;
    //             }
    //             break;
    //         }
    //         case YELLOW:
    //         {
    //             balls[i].g -= COLOR_SCALAR * delta_time;
    //             if (balls[i].g == 0)
    //             {
    //                 balls[i].c_state = RED;
    //                 break;
    //             }
    //             if (balls[i].g <= 0)
    //             {
    //                 balls[i].g = 0;
    //                 balls[i].c_state = RED;
    //                 break;
    //             }
    //             break;
    //         }
    //     }
    // }
};

void draw_game_objects()
{
    for (size_t i = 0; i < game_object_count; i++)
    {
        if (game_objects[i]->type == CIRCLE)
        {
            CircleObject* circle = (CircleObject*)game_objects[i];
            draw_circle_object(circle->base.position.x, circle->base.position.y, circle->radius,
                               circle->base.color.r, circle->base.color.g, circle->base.color.b);
        }
        else if (game_objects[i]->type == RECTANGLE)
        {
            RectObject* rect = (RectObject*)game_objects[i];
            draw_rect_object(rect->base.position.x, rect->base.position.y, rect->width,
                             rect->height, rect->base.color.r, rect->base.color.g,
                             rect->base.color.b);
        }
        else
        {
            printf("[ERROR] Unknown type of game object\n");
        }
    }
}

void draw_circle_object(int center_x, int center_y, int radius, float r, float g, float b)
{
    // Bresenham's Circle Algorithm
    int x = radius - 1;
    int y = 0;
    int dx = 1;
    int dy = 1;
    int err = dx - (radius << 1);
    SDL_SetRenderDrawColor(p_renderer, r, g, b, 255);

    while (x >= y)
    {
        SDL_RenderDrawPoint(p_renderer, center_x + x, center_y + y);
        SDL_RenderDrawPoint(p_renderer, center_x + y, center_y + x);
        SDL_RenderDrawPoint(p_renderer, center_x - y, center_y + x);
        SDL_RenderDrawPoint(p_renderer, center_x - x, center_y + y);
        SDL_RenderDrawPoint(p_renderer, center_x - x, center_y - y);
        SDL_RenderDrawPoint(p_renderer, center_x - y, center_y - x);
        SDL_RenderDrawPoint(p_renderer, center_x + y, center_y - x);
        SDL_RenderDrawPoint(p_renderer, center_x + x, center_y - y);

        if (err <= 0)
        {
            y++;
            err += dy;
            dy += 2;
        }
        if (err > 0)
        {
            x--;
            dx += 2;
            err += dx - (radius << 1);
        }
    }
    // fill the circle
    for (int i = -radius; i <= radius; i++)
    {
        for (int j = -radius; j <= radius; j++)
        {
            if (i * i + j * j <= radius * radius)
            {
                SDL_RenderDrawPoint(p_renderer, center_x + i, center_y + j);
            }
        }
    }
}

void draw_rect_object(int top_left_x, int top_left_y, int width, int height, float r, float g,
                      float b)
{
    SDL_Rect rect = {top_left_x, top_left_y, width, height};
    SDL_SetRenderDrawColor(p_renderer, r, g, b, 255);
    SDL_RenderFillRect(p_renderer, &rect);
}
void render()
{
    SDL_SetRenderDrawBlendMode(p_renderer, SDL_BLENDMODE_BLEND);
    SDL_SetRenderDrawColor(p_renderer, 0, 0, 0, 50);
    SDL_Rect fade_rect = {0, 0, WINDOW_WIDTH, WINDOW_HEIGHT};
    SDL_RenderFillRect(p_renderer, &fade_rect);

    for (size_t i = 0; i < atomic_load(&ball_count); i++)
    {
        SDL_SetRenderDrawColor(p_renderer, balls[i].r, balls[i].g, balls[i].b, 255);

        SDL_Rect ball_rect = convert_to_sdl_rect(&balls[i]);

        SDL_RenderFillRect(p_renderer, &ball_rect);
    }

    // call the draw function for each game_objects
    for (size_t i = 0; i < game_object_count; i++)
    {
        // game_objects[i]->draw(game_objects[i], p_renderer);
    }
    draw_game_objects();

    SDL_RenderPresent(p_renderer);
};

void destroy_window()
{
    SDL_DestroyRenderer(p_renderer);
    SDL_DestroyWindow(p_window);
    SDL_Quit();
};