#ifndef RENDER_H
#define RENDER_H
#include <SDL2/SDL.h>
#include <stdbool.h>
#include <Ball.h>
#include <BallHelper.h>
#include <main.h>
#include <Engine.h>

extern int WINDOW_WIDTH;
extern int WINDOW_HEIGHT;

extern SDL_Window* p_window;
extern SDL_Renderer* p_renderer;
extern bool game_is_running;
extern int last_frame_time;
extern float delta_time;

bool initialize_window(void);
void show_end_game_message_box_and_exit();
SDL_Rect convert_to_sdl_rect(struct ball* b);
void process_input();
void update();
void render();
void destroy_window();

#endif // RENDER_H