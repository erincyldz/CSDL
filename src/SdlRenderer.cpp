#include "SdlRenderer.hpp"

#include <algorithm>
#include <cmath>
#include <cstdio>
#include <unordered_set>
#include <vector>

namespace game::sdl
{
namespace
{
constexpr SDL_Color BACKGROUND{0, 0, 0, 255};
constexpr SDL_Color WHITE{255, 255, 255, 255};
constexpr SDL_Color GREY{170, 170, 170, 255};
constexpr SDL_Color HIGHLIGHT{255, 255, 0, 255};
constexpr SDL_Color TRAIL{255, 0, 0, 255};
constexpr SDL_Color ARROW{255, 255, 0, 255};

SDL_Color toSdl(object::Color c)
{
    return SDL_Color{c.r, c.g, c.b, c.a};
}

int roundToInt(double v)
{
    return static_cast<int>(std::lround(v));
}
}  // namespace

SdlRenderer::SdlRenderer(Window& window, const std::string& fontPath, int fontSize)
    : m_window(window), m_renderer(window.renderer())
{
    m_font.reset(TTF_OpenFont(fontPath.c_str(), fontSize));
    m_smallFont.reset(TTF_OpenFont(fontPath.c_str(), std::max(10, fontSize * 2 / 3)));
    if (!m_font || !m_smallFont)
        m_logger.warning("Failed to load font '{}': {} (text will not be shown)", fontPath,
                         TTF_GetError());
}

void SdlRenderer::beginFrame()
{
    SDL_SetRenderDrawBlendMode(m_renderer, SDL_BLENDMODE_NONE);
    setColor(BACKGROUND);
    SDL_RenderClear(m_renderer);
}

void SdlRenderer::endFrame()
{
    SDL_RenderPresent(m_renderer);
}

void SdlRenderer::drawWorld(const physics::PhysicsWorld& world, const RenderOptions& options)
{
    for (const auto& obj : world.objects())
    {
        drawObject(obj);
        if (options.trails)
            drawTrail(obj);
        if (options.direction)
            drawDirection(obj);
    }

    if (!options.collisionHighlights || world.activeContacts().empty())
        return;
    std::unordered_set<object::ObjectId> colliding;
    for (const auto& [a, b] : world.activeContacts())
    {
        colliding.insert(a);
        colliding.insert(b);
    }
    setColor(HIGHLIGHT);
    for (const auto& obj : world.objects())
        if (colliding.count(obj.getId()))
            drawShapeOutline(obj.getShape(), obj.getPosition(), 4);
}

void SdlRenderer::drawHud(const HudInfo& hud)
{
    const int w = hud.viewWidth > 0 ? hud.viewWidth : m_window.width();
    const int h = m_window.height();
    drawTextCentered("Collisions: " + std::to_string(hud.collisions), w / 2, 22, WHITE);
    if (hud.viewWidth > 0 && hud.viewWidth < m_window.width())
    {
        // Mark the right edge of the simulation area (objects bounce off it).
        setColor(SDL_Color{60, 60, 60, 255});
        SDL_RenderDrawLine(m_renderer, hud.viewWidth - 1, 0, hud.viewWidth - 1, h);
    }

    // FPS is rounded to 5 so the cached text doesn't change every frame.
    const long fps = std::lround(hud.fps / 5.0) * 5;
    drawText("Objects: " + std::to_string(hud.objects) + "   FPS: " + std::to_string(fps), 8,
             h - 44, GREY, true);
    drawText("P pause  N step  R reset  Q add  SPACE kick  DEL delete  F1 panel  ESC menu", 8,
             h - 24, GREY, true);
}

void SdlRenderer::drawMenu(const ui::Menu& menu)
{
    drawTextCentered(menu.title(), m_window.width() / 2, menu.titleY() + 20, WHITE);
    for (const auto& item : menu.items())
    {
        setColor(toSdl(item.color));
        const SDL_Rect r{item.rect.x, item.rect.y, item.rect.w, item.rect.h};
        SDL_RenderFillRect(m_renderer, &r);
        drawTextCentered(item.label, r.x + r.w / 2, r.y + r.h / 2, WHITE);
    }
}

void SdlRenderer::drawPausedOverlay(int viewWidth)
{
    // A banner instead of dimming the whole screen, so the scene stays readable while editing.
    const int w = viewWidth > 0 ? viewWidth : m_window.width();
    SDL_SetRenderDrawBlendMode(m_renderer, SDL_BLENDMODE_BLEND);
    SDL_SetRenderDrawColor(m_renderer, 0, 0, 0, 170);
    const SDL_Rect banner{0, 44, w, 36};
    SDL_RenderFillRect(m_renderer, &banner);
    SDL_SetRenderDrawBlendMode(m_renderer, SDL_BLENDMODE_NONE);
    drawTextCentered("PAUSED  (P resume, N step)", w / 2, 62, WHITE);
}

const SdlRenderer::CachedText* SdlRenderer::text(const std::string& str, SDL_Color color,
                                                 bool small)
{
    TTF_Font* font = small ? m_smallFont.get() : m_font.get();
    if (!font || str.empty())
        return nullptr;

    std::string key = str;
    key.push_back(small ? '\x1e' : '\x1f');
    key.append({static_cast<char>(color.r), static_cast<char>(color.g), static_cast<char>(color.b),
                static_cast<char>(color.a)});
    if (auto it = m_textCache.find(key); it != m_textCache.end())
        return &it->second;

    if (m_textCache.size() >= MAX_CACHED_TEXTS)
        m_textCache.clear();  // Dynamic strings (counters) would otherwise grow it forever.

    SurfacePtr surface(TTF_RenderUTF8_Blended(font, str.c_str(), color));
    if (!surface)
        return nullptr;
    TexturePtr texture(SDL_CreateTextureFromSurface(m_renderer, surface.get()));
    if (!texture)
        return nullptr;
    auto [it, inserted] =
        m_textCache.emplace(std::move(key), CachedText{std::move(texture), surface->w, surface->h});
    (void)inserted;
    return &it->second;
}

void SdlRenderer::drawText(const std::string& str, int x, int y, SDL_Color color, bool small)
{
    if (const CachedText* t = text(str, color, small))
    {
        const SDL_Rect dst{x, y, t->width, t->height};
        SDL_RenderCopy(m_renderer, t->texture.get(), nullptr, &dst);
    }
}

void SdlRenderer::drawTextCentered(const std::string& str, int centerX, int centerY,
                                   SDL_Color color)
{
    if (const CachedText* t = text(str, color))
    {
        const SDL_Rect dst{centerX - t->width / 2, centerY - t->height / 2, t->width, t->height};
        SDL_RenderCopy(m_renderer, t->texture.get(), nullptr, &dst);
    }
}

void SdlRenderer::setColor(SDL_Color c)
{
    SDL_SetRenderDrawColor(m_renderer, c.r, c.g, c.b, c.a);
}

void SdlRenderer::fillCircle(double cx, double cy, int radius)
{
    // Scanline fill: one line per row instead of one point per pixel.
    const int x = roundToInt(cx);
    const int y = roundToInt(cy);
    for (int dy = -radius; dy <= radius; ++dy)
    {
        const int dx = static_cast<int>(std::sqrt(radius * radius - dy * dy));
        SDL_RenderDrawLine(m_renderer, x - dx, y + dy, x + dx, y + dy);
    }
}

void SdlRenderer::ringOutline(double cx, double cy, int radius, int thickness)
{
    const int x = roundToInt(cx);
    const int y = roundToInt(cy);
    const int inner = radius - thickness;
    for (int dy = -radius; dy <= radius; ++dy)
    {
        const int outerDx = static_cast<int>(std::sqrt(radius * radius - dy * dy));
        if (inner > 0 && std::abs(dy) <= inner)
        {
            const int innerDx = static_cast<int>(std::sqrt(inner * inner - dy * dy));
            SDL_RenderDrawLine(m_renderer, x - outerDx, y + dy, x - innerDx, y + dy);
            SDL_RenderDrawLine(m_renderer, x + innerDx, y + dy, x + outerDx, y + dy);
        }
        else
        {
            SDL_RenderDrawLine(m_renderer, x - outerDx, y + dy, x + outerDx, y + dy);
        }
    }
}

void SdlRenderer::rectOutline(double left, double top, int width, int height, int thickness)
{
    for (int t = 0; t < thickness; ++t)
    {
        const SDL_Rect r{roundToInt(left) - t, roundToInt(top) - t, width + 2 * t, height + 2 * t};
        SDL_RenderDrawRect(m_renderer, &r);
    }
}

void SdlRenderer::drawObject(const object::GameObject& obj)
{
    const auto& p = obj.getPosition();
    setColor(toSdl(obj.getColor()));
    std::visit(object::Overloaded{[&](const object::CircleShape& c)
                                  {
                                      fillCircle(p.getX(), p.getY(), roundToInt(c.radius));
                                  },
                                  [&](const object::RectShape& r)
                                  {
                                      const SDL_FRect rect{
                                          static_cast<float>(p.getX() - r.width / 2.0),
                                          static_cast<float>(p.getY() - r.height / 2.0),
                                          static_cast<float>(r.width),
                                          static_cast<float>(r.height)};
                                      SDL_RenderFillRectF(m_renderer, &rect);
                                  }},
               obj.getShape());
}

void SdlRenderer::drawShapeOutline(const object::Shape& shape, const math::Vector2D& p,
                                   int thickness)
{
    std::visit(
        object::Overloaded{[&](const object::CircleShape& c)
                           {
                               ringOutline(p.getX(), p.getY(), roundToInt(c.radius), thickness);
                           },
                           [&](const object::RectShape& r)
                           {
                               rectOutline(p.getX() - r.width / 2.0, p.getY() - r.height / 2.0,
                                           roundToInt(r.width), roundToInt(r.height), thickness);
                           }},
        shape);
}

void SdlRenderer::drawSandboxOverlay(const physics::PhysicsWorld& world,
                                     const SandboxOverlay& overlay)
{
    // Grow a shape so its outline is drawn just outside the object.
    auto grown = [](object::Shape shape, double by)
    {
        std::visit(object::Overloaded{[&](object::CircleShape& c)
                                      {
                                          c.radius += by;
                                      },
                                      [&](object::RectShape& r)
                                      {
                                          r.width += 2 * by;
                                          r.height += 2 * by;
                                      }},
                   shape);
        return shape;
    };

    if (overlay.hovered && overlay.hovered != overlay.selected)
        if (const auto* obj = world.find(*overlay.hovered))
        {
            setColor(SDL_Color{200, 200, 200, 255});
            drawShapeOutline(grown(obj->getShape(), 2.0), obj->getPosition(), 1);
        }

    if (overlay.selected)
        if (const auto* obj = world.find(*overlay.selected))
        {
            setColor(SDL_Color{0, 255, 255, 255});
            drawShapeOutline(grown(obj->getShape(), 4.0), obj->getPosition(), 2);
        }

    if (overlay.ghostShape)
    {
        setColor(SDL_Color{120, 120, 120, 255});
        drawShapeOutline(*overlay.ghostShape, overlay.ghostPosition, 1);
    }

    if (overlay.dragLine)
    {
        const auto& [from, to] = *overlay.dragLine;
        setColor(WHITE);
        SDL_RenderDrawLineF(m_renderer, static_cast<float>(from.getX()),
                            static_cast<float>(from.getY()), static_cast<float>(to.getX()),
                            static_cast<float>(to.getY()));
    }
}

void SdlRenderer::drawTrail(const object::GameObject& obj)
{
    std::vector<SDL_FPoint> points;
    points.reserve(object::GameObject::TRAIL_LENGTH);
    obj.forEachTrailPoint(
        [&](const object::Position& p)
        {
            points.push_back(
                SDL_FPoint{static_cast<float>(p.getX()), static_cast<float>(p.getY())});
        });
    if (points.empty())
        return;
    setColor(TRAIL);
    SDL_RenderDrawPointsF(m_renderer, points.data(), static_cast<int>(points.size()));
}

void SdlRenderer::drawDirection(const object::GameObject& obj)
{
    const auto& pos = obj.getPosition();
    const auto& vel = obj.getVelocity();
    const double speed = vel.magnitude();
    if (speed < 0.5)
        return;  // No meaningful direction (e.g. stable objects).

    constexpr double MIN_ARROW_LENGTH = 20.0;
    constexpr double MAX_ARROW_LENGTH = 250.0;
    constexpr double HEAD_SIZE = 10.0;
    constexpr double HEAD_ANGLE = math::PI / 6.0;

    const double length = std::clamp(speed, MIN_ARROW_LENGTH, MAX_ARROW_LENGTH);
    const auto tip = pos + vel * (length / speed);
    const double angle = std::atan2(vel.getY(), vel.getX());

    setColor(ARROW);
    SDL_RenderDrawLineF(m_renderer, static_cast<float>(pos.getX()), static_cast<float>(pos.getY()),
                        static_cast<float>(tip.getX()), static_cast<float>(tip.getY()));
    for (const double a : {angle - HEAD_ANGLE, angle + HEAD_ANGLE})
    {
        SDL_RenderDrawLineF(m_renderer, static_cast<float>(tip.getX()),
                            static_cast<float>(tip.getY()),
                            static_cast<float>(tip.getX() - HEAD_SIZE * std::cos(a)),
                            static_cast<float>(tip.getY() - HEAD_SIZE * std::sin(a)));
    }
}
}  // namespace game::sdl
