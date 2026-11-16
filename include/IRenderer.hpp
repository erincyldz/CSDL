#pragma once
#include "Menu.hpp"
#include "PhysicsWorld.hpp"

#include <cstddef>
#include <cstdint>
#include <optional>
#include <utility>

namespace game
{
struct RenderOptions
{
    bool trails = true;
    bool direction = true;
    bool collisionHighlights = true;
};

struct HudInfo
{
    std::uint64_t collisions = 0;
    std::size_t objects = 0;
    double fps = 0.0;
    int viewWidth = 0;  // Width of the simulation area (window minus the docked panel)
};

// Editor feedback drawn on top of the world.
struct SandboxOverlay
{
    std::optional<object::ObjectId> selected;
    std::optional<object::ObjectId> hovered;
    std::optional<std::pair<math::Vector2D, math::Vector2D>> dragLine;  // Spawn velocity preview
    std::optional<object::Shape> ghostShape;  // Spawn tool preview under the cursor
    math::Vector2D ghostPosition;
};

// Backend-independent drawing interface used by Game.
class IRenderer
{
  public:
    virtual ~IRenderer() = default;

    virtual void beginFrame() = 0;
    virtual void drawWorld(const physics::PhysicsWorld& world, const RenderOptions& options) = 0;
    virtual void drawHud(const HudInfo& hud) = 0;
    virtual void drawSandboxOverlay(const physics::PhysicsWorld& world,
                                    const SandboxOverlay& overlay) = 0;
    virtual void drawMenu(const ui::Menu& menu) = 0;
    virtual void drawPausedOverlay(int viewWidth) = 0;
    virtual void endFrame() = 0;
};
}  // namespace game
