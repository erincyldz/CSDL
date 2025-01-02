#include <CircleObject.hpp>
#include <Game.hpp>
namespace game
{
Game::Game() : m_sdl(nullptr) {}

void Game::run()
{
    init();
    std::string loggerName = "CircleLogger";
    addGameObject(std::make_unique<game::object::CircleObject>(50.0f, loggerName));
    while (m_sdl->isRunning())
    {
        update();
        m_sdl->run(gameObjects);  // Pass game objects to SDLHelper for rendering
    }
    cleanup();
}

void Game::init()
{
    gameObjects = std::vector<std::unique_ptr<game::object::GameObject>>();

    std::string loggerName = "GameLogger";
    m_sdl = std::make_unique<game::sdl::SDLHelper>("Game Title", 800, 600, loggerName);
}

void Game::addGameObject(std::unique_ptr<game::object::GameObject> obj)
{
    gameObjects.push_back(std::move(obj));
}

int Game::getObjectCount() const
{
    return static_cast<int>(gameObjects.size());
}

// Update game state
// This function is called once per frame
// It is responsible for the backend logic of the game
void Game::update()
{
    for (size_t i = 0; i < gameObjects.size(); i++)
    {
        // gameObjects[i]->update(m_sdl->getDeltaTime());
        gameObjects[i]->update(m_sdl->getDeltaTime());
    }
}

void Game::cleanup()
{
    gameObjects.clear();
}
}  // namespace game