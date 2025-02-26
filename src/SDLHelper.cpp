#include "SDLHelper.hpp"

#include <CircleObject.hpp>
#include <RectObject.hpp>
#include <cmath>
#include <iostream>
namespace game::sdl
{
// Constructor
SDLHelper::SDLHelper(const std::string& title, int width, int height, std::string& logger_name)
    : m_logger(logger_name), m_window(nullptr), m_renderer(nullptr), m_isRunning(false),
      m_windowWidth(width), m_windowHeight(height), m_accumulator(0.0), m_lastFrameTime(0)
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
    font = TTF_OpenFont("./assets/fonts/SmartDuck.ttf", 24);
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
    TTF_Init();
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
    TTF_CloseFont(font);
    TTF_Quit();
    SDL_Quit();
    SDL_Log("SDLHelper cleaned up.");
}

// Handle events
void SDLHelper::handleEvents(GameState& state)
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
                    case SDLK_p:
                        if (state == GameState::PLAYING)
                        {
                            state = GameState::PAUSED;
                        }
                        else if (state == GameState::PAUSED)
                        {
                            state = GameState::PLAYING;
                        }
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
                break;
            case SDL_MOUSEBUTTONDOWN:
            {
                if (event.button.button == SDL_BUTTON_LEFT)
                {
                    int x = event.button.x;
                    int y = event.button.y;
                    if (state == GameState::MENU)
                    {
                        if (x > 200 && x < 400)
                        {
                            if (y > 100 && y < 150)
                            {
                                state = game::GameState::PLAYING;  // Play button
                            }
                            else if (y > 200 && y < 250)
                            {
                                state = game::GameState::PLAYING;  // Settings button TODO: ITS NOT
                                                                   // IMPLEMENTED YET SO I KEEP IT
                                                                   // IN PLAYING STATE
                            }
                            else if (y > 300 && y < 350)
                            {
                                state = game::GameState::EXIT;  // Exit button
                                m_isRunning = false;
                            }
                        }
                    }
                }
                break;
            }

            default:
                break;
        }
    }
}
void SDLHelper::renderMenu()
{
    SDL_Color playColor = {0, 255, 0, 255};
    SDL_Color settingsColor = {0, 0, 255, 255};
    SDL_Color exitColor = {255, 0, 0, 255};

    SDL_SetRenderDrawColor(m_renderer, 0, 0, 0, 255);
    SDL_RenderClear(m_renderer);

    SDL_SetRenderDrawColor(m_renderer, 255, 255, 255, 255);
    SDL_Rect playButton = {200, 100, 200, 50};
    SDL_Rect settingsButton = {200, 200, 200, 50};
    SDL_Rect exitButton = {200, 300, 200, 50};
    SDL_SetRenderDrawColor(m_renderer, playColor.r, playColor.g, playColor.g, playColor.a);
    SDL_RenderFillRect(m_renderer, &playButton);
    SDL_SetRenderDrawColor(m_renderer, settingsColor.r, settingsColor.g, settingsColor.g,
                           settingsColor.a);
    SDL_RenderFillRect(m_renderer, &settingsButton);
    SDL_SetRenderDrawColor(m_renderer, exitColor.r, exitColor.g, exitColor.g, exitColor.a);
    SDL_RenderFillRect(m_renderer, &exitButton);

    SDL_Color textColor = {255, 255, 255, 255};
    renderText(font, "Play", playButton.x + 70, playButton.y + 10, textColor);
    renderText(font, "Settings", settingsButton.x + 50, settingsButton.y + 10, textColor);
    renderText(font, "Exit", exitButton.x + 70, exitButton.y + 10, textColor);
}

void SDLHelper::renderPaused()
{
    // Set the blend mode to allow transparency
    SDL_SetRenderDrawBlendMode(m_renderer, SDL_BLENDMODE_BLEND);

    // Draw a semi-transparent black rectangle over the entire window
    SDL_SetRenderDrawColor(m_renderer, 0, 0, 0, 2);  // RGBA: Black with 200 alpha
    SDL_Rect overlay = {0, 0, m_windowWidth, m_windowHeight};
    SDL_RenderFillRect(m_renderer, &overlay);

    // Set text color
    SDL_Color textColor = {255, 255, 255, 255};  // White text

    // Render "PAUSED" text
    renderText(font, "PAUSED", m_windowWidth / 2 - 50, m_windowHeight / 2 - 25, textColor);
}

void SDLHelper::renderCollisionScoreboard(const int& collision_count)
{
    // Set text color
    SDL_Color textColor = {255, 255, 255, 255};  // White text

    std::string text = "Collision Count: " + std::to_string(collision_count);
    // Render collision count
    renderText(font, text, m_windowWidth / 2 - 50, 10, textColor);
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

void SDLHelper::renderText(TTF_Font* font, const std::string& text, int x, int y, SDL_Color color)
{
    SDL_Surface* surface = TTF_RenderText_Solid(font, text.c_str(), color);
    SDL_Texture* texture = SDL_CreateTextureFromSurface(m_renderer, surface);

    SDL_Rect dstRect = {x, y, surface->w, surface->h};
    SDL_RenderCopy(m_renderer, texture, nullptr, &dstRect);

    SDL_FreeSurface(surface);
    SDL_DestroyTexture(texture);
}

// Render the scene
void SDLHelper::render(const std::vector<std::unique_ptr<game::object::GameObject>>& gameObjects,
                       GameState& state)
{
    if (state == GameState::PLAYING || state == GameState::SETTINGS)
    {
        SDL_SetRenderDrawColor(m_renderer, 0, 0, 0, 255);  // Black background
        SDL_RenderClear(m_renderer);
        drawGameObjects(gameObjects);
    }
    else if (state == GameState::MENU)
    {
        this->renderMenu();
    }
    else if (state == GameState::PAUSED)
    {
        this->renderPaused();
    }
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
    auto rect_pos = rect.getPosition();
    auto rect_width = rect.get_width();
    auto rect_height = rect.get_height();
    int thickness = 5;
    SDL_SetRenderDrawColor(m_renderer, 255, 255, 0, 255);

    for (int t = 0; t < thickness; ++t)
    {
        // Draw the rectangle with increasing thickness
        SDL_Rect rect = {static_cast<int>(rect_pos.getX() - t),
                         static_cast<int>(rect_pos.getY() - t), rect_width + 2 * t,
                         rect_height + 2 * t};
        SDL_RenderDrawRect(m_renderer, &rect);
    }
}

void SDLHelper::drawOutline(const game::object::CircleObject& circle)
{
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
                SDL_RenderDrawPoint(m_renderer, circle_pos.getX() + dx, circle_pos.getY() + dy);
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
                    drawCircleFill(pos.getX(), pos.getY(), radius,
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
                    drawRectangleFill(pos.getX(), pos.getY(), rect->get_width(), rect->get_height(),
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
#endif

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

            if (obj->get_type() == game::object::ObjectType::CIRCLE)
            {
                drawOutline(*(dynamic_cast<game::object::CircleObject*>(obj)));
            }
            else if (obj->get_type() == game::object::ObjectType::RECTANGLE)
            {
                drawOutline(*(dynamic_cast<game::object::RectObject*>(obj)));
            }
        }
        m_sound->playSound();
    }
}

void SDLHelper::renderObjectDirection(const game::object::GameObject& obj)
{
    game::object::Position pos = obj.getCenter();
    game::object::Velocity vel = obj.getVelocity();

    // Normalize the velocity vector
    auto magnitude = vel.magnitude();

    // Fixed arrow length
    const double MAX_ARROW_LENGTH = 250;
    const double MIN_ARROW_LENGTH = 20;
    double arrow_length = std::clamp(magnitude, MIN_ARROW_LENGTH, MAX_ARROW_LENGTH);
    game::object::helper::Vector2D direction{0, 0};
    if (magnitude > 0)
    {
        direction = vel / magnitude;
    }

    // Scale the normalized vector to the fixed arrow length

    auto arrow_dir = pos + (direction * arrow_length);

    // Set arrow color
    SDL_Color color = {255, 255, 0, 255};  // Yellow
    SDL_SetRenderDrawColor(m_renderer, color.r, color.g, color.b, color.a);

    // Draw the main line of the arrow

    SDL_RenderDrawLine(m_renderer, pos.getX(), pos.getY(), arrow_dir.getX(), arrow_dir.getY());

    // Draw the arrowhead
    const double ARROWHEAD_SIZE = 10.0;
    const double ARROWHEAD_ANGLE = M_PI / 6;  // 30 degrees

    auto drawArrowhead = [&](double angle)
    {
        double arrowTipX = arrow_dir.getX() - ARROWHEAD_SIZE * std::cos(angle);
        double arrowTipY = arrow_dir.getY() - ARROWHEAD_SIZE * std::sin(angle);
        SDL_RenderDrawLine(m_renderer, arrow_dir.getX(), arrow_dir.getY(), arrowTipX, arrowTipY);
    };

    double angle = atan2(vel.getY(), vel.getX());  // Direction of the arrow
    drawArrowhead(angle - ARROWHEAD_ANGLE);
    drawArrowhead(angle + ARROWHEAD_ANGLE);
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
        SDL_RenderDrawPoint(m_renderer, pos.getX(), pos.getY());
    }
}

}  // namespace game::sdl
