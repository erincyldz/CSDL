#ifndef SDL_HELPER_HPP
#define SDL_HELPER_HPP

#include <ClassLogger.hpp>
#include <SDL2/SDL.h>
#include <stdexcept>
#include <string>
namespace game::sdl
{
#define DELTA_TIME_COFACTOR 1000.0f
#define FPS                 30
#define FRAME_TARGET_TIME   (1000 / FPS)

class SDLHelper
{
  public:
    SDLHelper(const std::string& title, int width, int height, std::string& logger_name);
    ~SDLHelper();

    // Main loop
    void run();

  protected:
    // Initialization and cleanup
    void initSDL();
    void cleanupSDL();

    // Event handling
    void handleEvents();
    void update(float deltaTime);
    void render();

    // Utility
    void drawCircle(int x, int y, int radius, SDL_Color color);
    void drawRectangle(int x, int y, int width, int height, SDL_Color color);
    void drawGameObjects();

    ClassLogger m_logger;

  private:
    // SDL-related members
    SDL_Window* m_window;
    SDL_Renderer* m_renderer;

    // Application state
    bool m_isRunning;
    int m_windowWidth;
    int m_windowHeight;
    float m_deltaTime;

    // Timing
    Uint32 m_lastFrameTime;
};
}  // namespace game::sdl
#endif  // SDL_HELPER_HPP