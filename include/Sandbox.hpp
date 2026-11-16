#pragma once
#include "GameObject.hpp"
#include "Input.hpp"
#include "PhysicsWorld.hpp"

#include <optional>
#include <random>

namespace game::sandbox
{
enum class Tool
{
    Select,  // Left: select / drag / throw objects
    Spawn    // Left: press to place, drag to give it a velocity
};

// Properties of objects created with the Spawn tool.
struct SpawnTemplate
{
    bool circle = true;
    double radius = 15.0;
    double width = 30.0;
    double height = 30.0;
    bool massFromArea = true;  // mass = area (same rule as random objects)
    double mass = 100.0;       // Used when massFromArea is false
    double restitution = 1.0;
    bool stable = false;
    bool randomColor = true;
    object::Color color{0, 200, 255, 255};
    double velocityPerPixel = 2.0;  // Drag length (px) -> initial speed (px/s)

    object::Shape shape() const;
};

struct DragLine
{
    math::Vector2D from;
    math::Vector2D to;
};

// Mouse interaction with the world. Independent of SDL and of the UI library so it can be
// driven from tests.
class Sandbox
{
  public:
    Tool tool = Tool::Select;
    SpawnTemplate spawn;
    bool throwOnRelease = true;  // Dragged objects keep the mouse velocity when released.

    std::optional<object::ObjectId> selected() const
    {
        return m_selected;
    }
    void select(std::optional<object::ObjectId> id)
    {
        m_selected = id;
    }

    void onMouseButton(physics::PhysicsWorld& world, const MouseButtonEvent& event,
                       std::mt19937& rng);
    void onMouseMove(int x, int y);
    // Moves the dragged object to the cursor; call once per frame (also while paused).
    void update(physics::PhysicsWorld& world, double frameDt);
    // Drops the selection / drag if the referenced object no longer exists.
    void validate(const physics::PhysicsWorld& world);

    bool deleteSelected(physics::PhysicsWorld& world);
    std::optional<object::ObjectId> duplicateSelected(physics::PhysicsWorld& world);

    object::GameObject makeFromTemplate(const math::Vector2D& position,
                                        const math::Vector2D& velocity, std::mt19937& rng) const;

    // For rendering.
    std::optional<DragLine> spawnDragLine() const;
    std::optional<object::ObjectId> hovered(const physics::PhysicsWorld& world) const;
    const math::Vector2D& mouse() const
    {
        return m_mouse;
    }
    bool isGrabbing() const
    {
        return m_grabbing;
    }

  private:
    std::optional<object::ObjectId> m_selected;
    math::Vector2D m_mouse;

    bool m_grabbing = false;
    bool m_grabPending = false;  // Pressed on an object, waiting for the mouse to move
    math::Vector2D m_pressPos;
    math::Vector2D m_grabOffset;
    math::Vector2D m_lastGrabTarget;
    math::Vector2D m_grabVelocity;

    std::optional<math::Vector2D> m_spawnStart;
    mutable int m_spawnCounter = 0;  // Only used to generate names
};
}  // namespace game::sandbox
