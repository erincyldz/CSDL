#include "SDLHelper.hpp"

#include <cmath>
#include <iostream>
namespace sdl
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
        if (event.type == SDL_QUIT)
            m_isRunning = false;
    }
}

// Update game state
void SDLHelper::update(float deltaTime)
{
    // Add update logic if needed
}

// Render the scene
void SDLHelper::render()
{
    SDL_SetRenderDrawColor(m_renderer, 0, 0, 0, 255);  // Black background
    SDL_RenderClear(m_renderer);

    // Draw shapes
    drawCircle(m_windowWidth / 2, m_windowHeight / 2, 50, {255, 0, 0, 255});  // Red circle
    drawRectangle(100, 100, 200, 150, {0, 255, 0, 255});                      // Green rectangle

    SDL_RenderPresent(m_renderer);
}

// Draw a circle
void SDLHelper::drawCircle(int x, int y, int radius, SDL_Color color)
{
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
}  // namespace sdl