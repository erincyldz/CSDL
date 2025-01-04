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
    std::string logger1Name = "Circle1Logger";
    auto circle1 = std::make_unique<game::object::CircleObject>(50.0f, logger1Name);
    game::object::Position pos = {300.0f, 400.0f};
    circle1->setVelocity({75.0f, -450.0f});
    circle1->setPosition(pos);
    circle1->setRestitution(1.0f);
    circle1->setMass(10.f);
    addGameObject(std::move(circle1));

    std::string logger2Name = "Circle2Logger";

    auto circle2 = std::make_unique<game::object::CircleObject>(50.0f, logger2Name);
    pos = {600.0f, 400.0f};
    circle2->setVelocity({-120.0f, 0.0f});
    circle2->setPosition(pos);
    circle2->setRestitution(1.0f);
    circle2->setMass(1.0f);

    addGameObject(std::move(circle2));

    addRandomGameObject();

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