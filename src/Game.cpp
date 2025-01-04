#include <CircleObject.hpp>
#include <Game.hpp>
#include <RectObject.hpp>
#define WINDOW_WIDTH  800
#define WINDOW_HEIGHT 600
namespace game
{
Game::Game() : m_sdl(nullptr) {}

void Game::run()
{
    init();
    std::string loggerName = "CircleLogger";
    auto circle1 = std::make_unique<game::object::CircleObject>(50.0f, loggerName);
    game::object::Position pos = {100.0f, 100.0f};
    circle1->setVelocity({200.0f, 100.0f});
    circle1->setPosition(pos);
    addGameObject(std::move(circle1));

    auto rect1 = std::make_unique<game::object::RectObject>(100.0f, 100.0f, loggerName);
    pos = {200.0f, 200.0f};
    rect1->setVelocity({100.0f, 200.0f});
    rect1->setPosition(pos);
    addGameObject(std::move(rect1));

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