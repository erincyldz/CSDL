#ifndef GAME_H
#define GAME_H
#include <CollisionManager.hpp>
#include <GameHelper.hpp>
#include <GameObject.hpp>
#include <IRenderer.hpp>
#include <SDLHelper.hpp>
#include <iomanip>  // For std::setw
#include <memory>
#include <vector>

namespace game
{
class Game
{
  public:
    Game();
    ~Game() = default;  // Use default destructor since smart pointers handle cleanup
    void run();
    void addGameObject(std::unique_ptr<game::object::GameObject> obj);
    void addRandomGameObject();
    void addRandomGameObject(int num_objects);
    int getObjectCount() const;  // Retrieve the count of game objects
    void setWindowWidth(int width)
    {
        m_window_width = width;
    }
    void setWindowHeight(int height)
    {
        m_window_height = height;
    }
    int getWindowWidth() const
    {
        return m_window_width;
    }
    int getWindowHeight() const
    {
        return m_window_height;
    }
    GameState* p_gameState;

  private:
    void init();
    void update();
    void cleanup();

    std::unique_ptr<game::sdl::SDLHelper> m_sdl;
    std::vector<std::unique_ptr<game::object::GameObject>> gameObjects;  // Vector of game objects
    game::engine::CollisionManager m_collisionManager;

    int m_window_width = 600;
    int m_window_height = 600;
    static constexpr double m_LOGIC_TIMESTEP = 1.0 / FPS;  // 60 updates per second
};
}  // namespace game

#endif  // GAME_H