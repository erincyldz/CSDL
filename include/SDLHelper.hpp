#ifndef SDL_HELPER_HPP
#define SDL_HELPER_HPP

#include <ClassLogger.hpp>
#include <GameObject.hpp>
#include <IRenderer.hpp>
#include <SDL2/SDL.h>
#include <SDLSound.hpp>
#include <memory>
#include <stdexcept>
#include <string>
#include <vector>
#define DELTA_TIME_COFACTOR 1000.0f
#define FPS                 60
#define FRAME_TARGET_TIME   (1000 / FPS)

#define RENDER_LAST_POSITIONS true
namespace game::sdl
{

class SDLHelper : public game::IRenderer
{
  public:
    SDLHelper(const std::string& title, int width, int height, std::string& logger_name);
    ~SDLHelper();

    // rendering
    void present();  // Call SDL_RenderPresent once here
    // Main loop
    float getDeltaTime();
    double getAccumulator() const;

    bool isRunning() const;
    std::pair<int, int> getScreenDim();
    void renderCollisionHighlights(
        const std::vector<std::pair<game::object::GameObject*, game::object::GameObject*>>&
            collisions) override;

    void renderObjectLastPosition(const std::unique_ptr<game::object::GameObject>& gameObject);

    sound::Sound* m_sound;

    // Event handling
    void handleEvents();
    void update();
    void render(const std::vector<std::unique_ptr<game::object::GameObject>>& gameObjects);
    void reduceAccumulator(double timestep);

  protected:
    // Initialization and cleanup
    void initSDL();
    void cleanupSDL();

    // Utility
    void drawCircleFill(int x, int y, int radius, SDL_Color color);
    void drawRectangleFill(int x, int y, int width, int height, SDL_Color color);
    void drawOutline(const game::object::RectObject& rect);
    void drawOutline(const game::object::CircleObject& circle);
    void drawGameObjects(const std::vector<std::unique_ptr<game::object::GameObject>>& gameObjects);

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
    float m_accumulator;

    // Timing
    Uint32 m_lastFrameTime;
};
}  // namespace game::sdl
#endif  // SDL_HELPER_HPP