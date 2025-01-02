#include "SDLHelper.hpp"

#include <cmath>
#include <iostream>
namespace game::sdl
{
// Constructor
SDLHelper::SDLHelper(const std::string& title, int width, int height, std::string& logger_name)
    : m_window(nullptr), m_renderer(nullptr), m_isRunning(false), m_windowWidth(width),
      m_windowHeight(height), m_lastFrameTime(0), m_logger(logger_name)
{
    SDL_Log("Initializing SDLHelper...");
    initSDL();

    m_window = SDL_CreateWindow(title.c_str(), SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED,
                                width, height, SDL_WINDOW_SHOWN);
    if (!m_window)
        throw std::runtime_error("Failed to create window: " + std::string(SDL_GetError()));

    m_renderer =
        SDL_CreateRenderer(m_window, -1, SDL_RENDERER_ACCELERATED | SDL_RENDERER_PRESENTVSYNC);
    if (!m_renderer)
        throw std::runtime_error("Failed to create renderer: " + std::string(SDL_GetError()));

    m_isRunning = true;
    SDL_Log("SDLHelper initialized successfully.");
}

// Destructor
SDLHelper::~SDLHelper()
{
    cleanupSDL();
}

// Run the main loop
void SDLHelper::run()
{
    while (m_isRunning)
    {
        Uint32 currentFrameTime = SDL_GetTicks();
        float deltaTime = (currentFrameTime - m_lastFrameTime) / 1000.0f;
        m_lastFrameTime = currentFrameTime;

        handleEvents();
        update(deltaTime);
        render();
    }
}

// Initialize SDL
void SDLHelper::initSDL()
{
    if (SDL_Init(SDL_INIT_VIDEO) != 0)
        throw std::runtime_error("Failed to initialize SDL: " + std::string(SDL_GetError()));
}

// Cleanup SDL
void SDLHelper::cleanupSDL()
{
    if (m_renderer)
        SDL_DestroyRenderer(m_renderer);
    if (m_window)
        SDL_DestroyWindow(m_window);
    SDL_Quit();
    SDL_Log("SDLHelper cleaned up.");
}

// Handle events
void SDLHelper::handleEvents()
{
    SDL_Event event;
    while (SDL_PollEvent(&event))
    {
        switch (event.type)
        {
            case SDL_QUIT:
                m_isRunning = false;
                std::cout << "[SDL_QUIT] quitted" << std::endl;
                break;

            case SDL_KEYDOWN:
                switch (event.key.keysym.sym)
                {
                    case SDLK_ESCAPE:
                        m_isRunning = false;
                        std::cout << "[Q] quitted" << std::endl;
                        break;

                    case SDLK_q:
                        // TODO: add random game object
                        break;

                    default:
                        break;
                }

            default:
                break;
        }
    }
}

// Update game state
void SDLHelper::update(float deltaTime)
{
    int time_to_wait = FRAME_TARGET_TIME - (SDL_GetTicks() - m_lastFrameTime);
    if (time_to_wait > 0 && time_to_wait <= FRAME_TARGET_TIME)
    {
        SDL_Delay(time_to_wait);
    }
    m_deltaTime = (SDL_GetTicks() - m_lastFrameTime) / DELTA_TIME_COFACTOR;

    // TODO: call the update function for each game_objects
    //  for(size_t i = 0; i < game_objects.size(); i++)
    //  {
    //      game_objects[i]->update(m_deltaTime);
    //  }
}

// Render the scene
void SDLHelper::render()
{
    SDL_SetRenderDrawColor(m_renderer, 0, 0, 0, 255);  // Black background
    SDL_RenderClear(m_renderer);

    // Draw game objects

    SDL_RenderPresent(m_renderer);
}

// Draw a circle
void SDLHelper::drawCircle(int x, int y, int radius, SDL_Color color)
{
    // Midpoint circle algorithm
    SDL_SetRenderDrawColor(m_renderer, color.r, color.g, color.b, color.a);

    for (int w = 0; w < radius * 2; w++)
    {
        for (int h = 0; h < radius * 2; h++)
        {
            int dx = radius - w;
            int dy = radius - h;
            if ((dx * dx + dy * dy) <= (radius * radius))
            {
                SDL_RenderDrawPoint(m_renderer, x + dx, y + dy);
            }
        }
    }
}

// Draw a rectangle
void SDLHelper::drawRectangle(int x, int y, int width, int height, SDL_Color color)
{
    SDL_SetRenderDrawColor(m_renderer, color.r, color.g, color.b, color.a);
    SDL_Rect rect = {x, y, width, height};
    SDL_RenderFillRect(m_renderer, &rect);
}


void SDLHelper::drawGameObjects()
{
    // for (size_t i = 0; i < game_objects.size(); i++)
    // {
    //     switch (game_objects[i]->get_type())
    //     {
    //         case GameObjectType::CIRCLE:
    //             drawCircle(game_objects[i]->get_x(), game_objects[i]->get_y(),
    //                        game_objects[i]->get_radius(), game_objects[i]->get_color());
    //             break;

    //         case GameObjectType::RECTANGLE:
    //             drawRectangle(game_objects[i]->get_x(), game_objects[i]->get_y(),
    //                           game_objects[i]->get_width(), game_objects[i]->get_height(),
    //                           game_objects[i]->get_color());
    //             break;

    //         default:
    //             break;
    //     }
    // }
}

}  // namespace sdl
