#include "SDLHelper.hpp"

#include <CircleObject.hpp>
#include <RectObject.hpp>
#include <cmath>
#include <iostream>
namespace game::sdl
{
// Constructor
SDLHelper::SDLHelper(const std::string& title, int width, int height, std::string& logger_name)
    : m_window(nullptr), m_renderer(nullptr), m_isRunning(false), m_windowWidth(width),
      m_windowHeight(height), m_lastFrameTime(0), m_logger(logger_name)
{
    SDL_Log("Initializing SDLHelper...");
    initSDL();

    m_window = SDL_CreateWindow(title.c_str(), SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED,
                                width, height, SDL_WINDOW_RESIZABLE);
    if (!m_window)
        throw std::runtime_error("Failed to create window: " + std::string(SDL_GetError()));

    m_renderer =
        SDL_CreateRenderer(m_window, -1, SDL_RENDERER_ACCELERATED | SDL_RENDERER_PRESENTVSYNC);
    if (!m_renderer)
        throw std::runtime_error("Failed to create renderer: " + std::string(SDL_GetError()));

    m_accumulator = 0.0;
    m_isRunning = true;
    SDL_Log("SDLHelper initialized successfully.");
}

// Destructor
SDLHelper::~SDLHelper()
{
    cleanupSDL();
}

void SDLHelper::present()
{
    SDL_RenderPresent(m_renderer);
}

// Initialize SDL
void SDLHelper::initSDL()
{
    if (SDL_Init(SDL_INIT_VIDEO | SDL_INIT_AUDIO) != 0)
        throw std::runtime_error("Failed to initialize SDL: " + std::string(SDL_GetError()));
    m_sound = new sound::Sound();
    // m_sound->SetupDevice();
    m_sound->initMixer();
}

bool SDLHelper::isRunning() const
{
    return m_isRunning;
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
        switch (event.type)
        {
            case SDL_QUIT:
                m_isRunning = false;
                break;

            case SDL_WINDOWEVENT:
                switch (event.window.event)
                {
                    case SDL_WINDOWEVENT_RESIZED:
                        m_windowWidth = event.window.data1;
                        m_windowHeight = event.window.data2;
                        break;

                    default:
                        break;
                }
                break;

            case SDL_KEYDOWN:
                switch (event.key.keysym.sym)
                {
                    case SDLK_ESCAPE:
                        m_isRunning = false;
                        break;

                    case SDLK_q:
                        // TODO: add random game object
                        break;

                    case SDLK_r:
                        // reset game object velocities
                        m_isResetObjects = true;
                        break;
                    case SDLK_SPACE:
                        m_isAddForce = true;
                        break;
                    default:
                        break;
                }

            default:
                break;
        }
    }
}

// Update game state
void SDLHelper::update()
{
    // Get current frame time
    uint32_t currentTime = SDL_GetTicks();
    uint32_t elapsedTime = currentTime - m_lastFrameTime;

    // Delay to maintain target frame rate
    int time_to_wait = FRAME_TARGET_TIME - elapsedTime;
    if (time_to_wait > 0 && time_to_wait <= FRAME_TARGET_TIME)
    {
        SDL_Delay(time_to_wait);
    }

    // Calculate delta time
    elapsedTime = SDL_GetTicks() - m_lastFrameTime;  // Recalculate after potential delay
    m_lastFrameTime = SDL_GetTicks();
    m_deltaTime = elapsedTime / DELTA_TIME_COFACTOR;  // Convert to seconds

    // Accumulate time for fixed timestep
    m_accumulator += m_deltaTime;
}

// Render the scene
void SDLHelper::render(const std::vector<std::unique_ptr<game::object::GameObject>>& gameObjects)
{
    SDL_SetRenderDrawColor(m_renderer, 0, 0, 0, 255);  // Black background
    SDL_RenderClear(m_renderer);
    drawGameObjects(gameObjects);
}

// Draw a circle
void SDLHelper::drawCircleFill(int x, int y, int radius, SDL_Color color)
{
    // Midpoint circle algorithm
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
void SDLHelper::drawOutline(const game::object::RectObject& rect)
{
    auto rect_color = rect.get_color();
    auto rect_pos = rect.getPosition();
    auto rect_width = rect.get_width();
    auto rect_height = rect.get_height();
    auto rect_dim = rect.getDimensions();
    int thickness = 5;
    SDL_SetRenderDrawColor(m_renderer, 255, 255, 0, 255);

    for (int t = 0; t < thickness; ++t)
    {
        // Draw the rectangle with increasing thickness
        SDL_Rect rect = {static_cast<int>(rect_pos.x - t), static_cast<int>(rect_pos.y - t),
                         rect_width + 2 * t, rect_height + 2 * t};
        SDL_RenderDrawRect(m_renderer, &rect);
    }
}

void SDLHelper::drawOutline(const game::object::CircleObject& circle)
{
    auto circle_color = circle.get_color();
    SDL_SetRenderDrawColor(m_renderer, 255, 255, 0, 255);
    auto radius = circle.getRadius();
    int thickness = 5;
    auto circle_pos = circle.getPosition();
    for (int w = 0; w < radius * 2; w++)
    {
        for (int h = 0; h < radius * 2; h++)
        {
            int dx = radius - w;
            int dy = radius - h;
            int distanceSquared = dx * dx + dy * dy;

            // Check if the point lies within the outline range
            if (distanceSquared <= (radius * radius) &&
                distanceSquared >= ((radius - thickness) * (radius - thickness)))
            {
                SDL_RenderDrawPoint(m_renderer, circle_pos.x + dx, circle_pos.y + dy);
            }
        }
    }
}

// Draw a rectangle
void SDLHelper::drawRectangleFill(int x, int y, int width, int height, SDL_Color color)
{
    SDL_SetRenderDrawColor(m_renderer, color.r, color.g, color.b, color.a);
    SDL_Rect rect = {x, y, width, height};
    SDL_RenderFillRect(m_renderer, &rect);
}

void SDLHelper::drawGameObjects(
    const std::vector<std::unique_ptr<game::object::GameObject>>& gameObjects)
{
    for (const auto& obj : gameObjects)
    {
        game::object::Position pos = obj->getPosition();

        switch (obj->get_type())
        {
            case game::object::helper::ObjectType::CIRCLE:
                // Safely cast to CircleObject
                if (auto* circle = dynamic_cast<game::object::CircleObject*>(obj.get()))
                {
                    int radius = static_cast<int>(circle->getRadius());
                    drawCircleFill(pos.x, pos.y, radius,
                                   {
                                       static_cast<Uint8>(obj->get_color().r),
                                       static_cast<Uint8>(obj->get_color().g),
                                       static_cast<Uint8>(obj->get_color().b),
                                       255  // Fully opaque
                                   });
                }
                break;

            case game::object::helper::ObjectType::RECTANGLE:
                // Safely cast to CircleObject
                if (auto* rect = dynamic_cast<game::object::RectObject*>(obj.get()))
                {
                    drawRectangleFill(pos.x, pos.y, rect->get_width(), rect->get_height(),
                                      {
                                          static_cast<Uint8>(obj->get_color().r),
                                          static_cast<Uint8>(obj->get_color().g),
                                          static_cast<Uint8>(obj->get_color().b),
                                          255  // Fully opaque
                                      });
                }

            default:
                break;
        }


#if RENDER_LAST_POSITIONS
        renderObjectLastPosition(obj);

#if RENDER_DIRECTION
        renderObjectDirection(*obj);

#endif
    }
}

float SDLHelper::getDeltaTime()
{
    return m_deltaTime;
}
std::pair<int, int> SDLHelper::getScreenDim()
{
    return std::pair<int, int>(m_windowWidth, m_windowHeight);
}

void SDLHelper::renderCollisionHighlights(
    const std::vector<std::pair<game::object::GameObject*, game::object::GameObject*>>& collisions)
{
    SDL_Color color = {255, 255, 0, 255};                                    // Yellow
    SDL_SetRenderDrawColor(m_renderer, color.r, color.g, color.b, color.a);  // Yellow
    for (const auto& collision : collisions)
    {
        for (auto* obj : {collision.first, collision.second})
        {

            if (obj->m_type == game::object::ObjectType::CIRCLE)
            {
                drawOutline(*(dynamic_cast<game::object::CircleObject*>(obj)));
            }
            else if (obj->m_type == game::object::ObjectType::RECTANGLE)
            {
                drawOutline(*(dynamic_cast<game::object::RectObject*>(obj)));
            }
        }
        m_sound->playSound();
    }
}

void SDLHelper::renderObjectDirection(const game::object::GameObject& obj)
{
    game::object::Position pos = obj.getPosition();
    game::object::Velocity vel = obj.getVelocity();

    // Fixed arrow length
    const double ARROW_LENGTH = 50.0;

    // Normalize the velocity vector
    double magnitude = std::sqrt(vel.x * vel.x + vel.y * vel.y);
    double dirX = (magnitude > 0) ? vel.x / magnitude : 0.0;
    double dirY = (magnitude > 0) ? vel.y / magnitude : 0.0;

    // Scale the normalized vector to the fixed arrow length
    double endX = pos.x + dirX * ARROW_LENGTH;
    double endY = pos.y + dirY * ARROW_LENGTH;

    // Set arrow color
    SDL_Color color = {255, 255, 0, 255};  // Yellow
    SDL_SetRenderDrawColor(m_renderer, color.r, color.g, color.b, color.a);

    // Draw the main line of the arrow
    SDL_RenderDrawLine(m_renderer, pos.x, pos.y, endX, endY);

    // Draw the arrowhead
    const double ARROWHEAD_SIZE = 10.0;
    const double ARROWHEAD_ANGLE = M_PI / 6;  // 30 degrees

    auto drawArrowhead = [&](double baseX, double baseY, double angle)
    {
        double arrowTipX = endX - ARROWHEAD_SIZE * std::cos(angle);
        double arrowTipY = endY - ARROWHEAD_SIZE * std::sin(angle);
        SDL_RenderDrawLine(m_renderer, endX, endY, arrowTipX, arrowTipY);
    };

    double angle = atan2(dirY, dirX);  // Direction of the arrow
    drawArrowhead(endX, endY, angle - ARROWHEAD_ANGLE);
    drawArrowhead(endX, endY, angle + ARROWHEAD_ANGLE);
}

double SDLHelper::getAccumulator() const
{
    return m_accumulator;
}
void SDLHelper::reduceAccumulator(double timestep)
{
    m_accumulator -= timestep;
}

void SDLHelper::renderObjectLastPosition(
    const std::unique_ptr<game::object::GameObject>& gameObject)
{
    // render the last position vector
    auto last_pos = gameObject->getLastPositions();
    SDL_SetRenderDrawColor(m_renderer, 255, 0, 0, 255);  // Red color for last positions
    for (const auto& pos : last_pos)
    {
        SDL_RenderDrawPoint(m_renderer, pos.x, pos.y);
    }
}

}  // namespace game::sdl
