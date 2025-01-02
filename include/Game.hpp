#ifndef GAME_H
#define GAME_H
#include <GameObject.hpp>
#include <SDLHelper.hpp>
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
    int getObjectCount() const;  // Retrieve the count of game objects

  private:
    void init();
    void update();
    void cleanup();

    std::unique_ptr<game::sdl::SDLHelper> m_sdl;
    std::vector<std::unique_ptr<game::object::GameObject>> gameObjects;  // Vector of game objects
};
}  // namespace game

#endif  // GAME_H