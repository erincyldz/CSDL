#include <CircleObject.hpp>
#include <Game.hpp>
#include <RectObject.hpp>
#include <random>  // For modern random number generation

#define PI 3.14159265358979323846

namespace game
{
Game::Game() : m_sdl(nullptr)
{
    init();
}

void Game::run()
{
    std::cout << "Game is running" << std::endl;
    while (m_sdl->isRunning())
    {

        m_sdl->update();  // Update SDL timing

        m_sdl->handleEvents(*p_gameState);
        // Fixed timestep for game logic
        if (*p_gameState == game::GameState::PLAYING)
        {
            while (m_sdl->getAccumulator() >= m_LOGIC_TIMESTEP)
            {
                update();
                m_sdl->reduceAccumulator(m_LOGIC_TIMESTEP);
            }
        }

        m_sdl->render(gameObjects, *p_gameState);  // Pass game objects to SDLHelper for rendering
        // m_sdl->renderCollisionHighlights(m_collisionManager.get_active_collisions());
        m_sdl->present();
    }
    cleanup();
}

void Game::init()
{
    gameObjects = std::vector<std::unique_ptr<game::object::GameObject>>();
    *p_gameState = GameState::MENU;
    std::string loggerName = "sdl_logger";
    m_sdl = std::make_unique<game::sdl::SDLHelper>("Game Title", m_window_width, m_window_height,
                                                   loggerName);
}

void Game::addGameObject(std::unique_ptr<game::object::GameObject> obj)
{
    gameObjects.push_back(std::move(obj));
}

void Game::addRandomGameObject()
{
    // get a random number between 0 and 1
    // Create a random number generator
    static std::random_device rd;                                       // Seed
    static std::mt19937 gen(rd());                                      // Mersenne Twister engine
                                                                        // Define distributions
    std::uniform_int_distribution<> randomX(10, m_window_width - 20);   // X position
    std::uniform_int_distribution<> randomY(10, m_window_height - 20);  // Y position

    std::string loggerName;
    // Create a random number between 0 and 1
    float random = static_cast<float>(rand()) / static_cast<float>(RAND_MAX);

    if (random > 0.5f)
    {
        loggerName = "RandomCircleObjectLogger";
        std::uniform_int_distribution<> randomRadius(10, 50);  // Radius between 10 and 50

        auto circle = std::make_unique<game::object::CircleObject>(loggerName);
        // Set properties
        circle->setRadius(randomRadius(gen));
        game::object::Position pos;
        pos.x = randomX(gen);
        pos.y = randomY(gen);
        circle->setPosition(pos);
        circle->setMass(PI * circle->getRadius() * circle->getRadius());

        // Optionally, set velocity, color, or other properties
        circle->setVelocity({
            static_cast<float>(randomX(gen) % 100 - 200),  // Random velocity x: -50 to 50
            static_cast<float>(randomY(gen) % 100 - 200)   // Random velocity y: -50 to 50
        });
        addGameObject(std::move(circle));
    }
    else
    {
        loggerName = "RandomRectObjectLogger";
        std::uniform_int_distribution<> randomDimension(10, 50);  // Width/Height between 10 and 50

        auto rect = std::make_unique<game::object::RectObject>(loggerName);
        // Set properties
        rect->setDimensions(randomDimension(gen), randomDimension(gen));
        game::object::Position pos;
        pos.x = randomX(gen);
        pos.y = randomY(gen);
        rect->setPosition(pos);
        rect->setMass(rect->getDimensions().first * rect->getDimensions().second);
        // Optionally, set velocity, color, or other properties
        rect->setVelocity({
            static_cast<float>(randomX(gen) % 100 - 200),  // Random velocity x: -50 to 50
            static_cast<float>(randomY(gen) % 100 - 200)   // Random velocity y: -50 to 50
        });

        addGameObject(std::move(rect));
    }
}

void Game::addRandomGameObject(int num_objects)
{
    if (num_objects <= 0)
    {
        return;
    }
    if (num_objects > MAX_GAME_OBJECTS)
    {
        num_objects = MAX_GAME_OBJECTS;
    }
    for (int i = 0; i < num_objects; ++i)
    {
        addRandomGameObject();
    }
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
    // if (m_sdl->m_isResetObjects)
    // {
    //     std::cout << "Resetting game objects" << std::endl;
    //     for (auto& object : gameObjects)
    //     {
    //         object->setVelocity({0, 0});
    //         object->setAcceleration({0, 0});
    //         object->setForce({0, 0});
    //     }
    //     m_sdl->m_isResetObjects = false;
    // }

    // if (m_sdl->m_isAddForce)
    // {
    //     for (auto& object : gameObjects)
    //     {
    //         object->addForce({0, 20});
    //     }
    //     m_sdl->m_isAddForce = false;
    // }

    auto screenDim = m_sdl->getScreenDim();
    m_collisionManager.calculate_gravitational_force(gameObjects);

    for (auto& object : gameObjects)
    {
        object->update(m_LOGIC_TIMESTEP, screenDim.first, screenDim.second);
    }
    m_collisionManager.resolve_collisions(gameObjects);
}

void Game::cleanup()
{
    gameObjects.clear();
}
}  // namespace game