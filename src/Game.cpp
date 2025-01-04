#include <CircleObject.hpp>
#include <Game.hpp>
#include <RectObject.hpp>
#define WINDOW_WIDTH  800
#define WINDOW_HEIGHT 600
namespace game
{
Game::Game() : m_sdl(nullptr)
{
    init();
}

void Game::run()
{
    while (m_sdl->isRunning())
    {
        update();
        m_sdl->run(gameObjects);  // Pass game objects to SDLHelper for rendering
        m_sdl->renderCollisionHighlights(m_collisionManager.get_active_collisions());
    }
    cleanup();
}

void Game::init()
{
    gameObjects = std::vector<std::unique_ptr<game::object::GameObject>>();

    std::string loggerName = "GameLogger";
    m_sdl = std::make_unique<game::sdl::SDLHelper>("Game Title", WINDOW_WIDTH, WINDOW_HEIGHT,
                                                   loggerName);
}

void Game::addGameObject(std::unique_ptr<game::object::GameObject> obj)
{
    gameObjects.push_back(std::move(obj));
}

void Game::addRandomGameObject()
{
    // TODO: Implement this function
    std::string loggerName = "RandomCircleObjectLogger";
    auto circle = std::make_unique<game::object::CircleObject>(15.0f, loggerName);
    loggerName = "RandomRectObjectLogger";
    auto rect = std::make_unique<game::object::RectObject>(loggerName);
    addGameObject(std::move(circle));
    addGameObject(std::move(rect));
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
    auto screenDim = m_sdl->getScreenDim();
    for (auto& object : gameObjects)
    {
        object->update(m_sdl->getDeltaTime(), screenDim.first, screenDim.second);
    }
    m_collisionManager.resolve_collisions(gameObjects);
}

void Game::cleanup()
{
    gameObjects.clear();
}
}  // namespace game