#include "Sandbox.hpp"

#include "Random.hpp"

#include <string>

namespace game::sandbox
{
using math::Vector2D;
using object::GameObject;

namespace
{
constexpr double MIN_SPAWN_DRAG_PX = 3.0;  // Shorter drags spawn the object at rest
constexpr double GRAB_VELOCITY_SMOOTHING = 0.35;
constexpr double GRAB_THRESHOLD_PX = 3.0;  // Mouse travel before a click becomes a drag
}  // namespace

object::Shape SpawnTemplate::shape() const
{
    if (circle)
        return object::CircleShape{radius};
    return object::RectShape{width, height};
}

void Sandbox::onMouseButton(physics::PhysicsWorld& world, const MouseButtonEvent& event,
                            std::mt19937& rng)
{
    const Vector2D p{static_cast<double>(event.x), static_cast<double>(event.y)};
    m_mouse = p;

    if (event.button == MouseButton::Right)
    {
        // Right click deletes the object under the cursor, whatever the tool.
        if (event.pressed)
        {
            if (const auto id = world.objectAt(p))
            {
                world.remove(*id);
                validate(world);
            }
        }
        return;
    }

    if (tool == Tool::Select)
    {
        if (event.pressed)
        {
            // A plain click only selects; the grab starts once the mouse has moved a little
            // (see update()), so inspecting an object doesn't disturb its motion.
            m_selected = world.objectAt(p);
            if (m_selected)
            {
                m_grabPending = true;
                m_pressPos = p;
                m_grabOffset = p - world.find(*m_selected)->getPosition();
            }
        }
        else
        {
            m_grabPending = false;
            if (m_grabbing)
            {
                m_grabbing = false;
                if (GameObject* obj = m_selected ? world.find(*m_selected) : nullptr)
                    obj->setVelocity(throwOnRelease ? m_grabVelocity : Vector2D{});
            }
        }
        return;
    }

    // Spawn tool
    if (event.pressed)
    {
        m_spawnStart = p;
    }
    else if (m_spawnStart)
    {
        const Vector2D drag = p - *m_spawnStart;
        const Vector2D velocity =
            drag.magnitude() < MIN_SPAWN_DRAG_PX ? Vector2D{} : drag * spawn.velocityPerPixel;
        m_selected = world.add(makeFromTemplate(*m_spawnStart, velocity, rng));
        m_spawnStart.reset();
    }
}

void Sandbox::onMouseMove(int x, int y)
{
    m_mouse = Vector2D{static_cast<double>(x), static_cast<double>(y)};
}

void Sandbox::update(physics::PhysicsWorld& world, double frameDt)
{
    if (!m_grabbing && !m_grabPending)
        return;
    GameObject* obj = m_selected ? world.find(*m_selected) : nullptr;
    if (!obj)
    {
        m_grabbing = false;
        m_grabPending = false;
        return;
    }
    if (m_grabPending)
    {
        if ((m_mouse - m_pressPos).magnitude() < GRAB_THRESHOLD_PX)
            return;
        m_grabPending = false;
        m_grabbing = true;
        m_lastGrabTarget = obj->getPosition();
        m_grabVelocity = {};
    }

    const Vector2D target = m_mouse - m_grabOffset;
    if (frameDt > 0.0)
    {
        const Vector2D instant = (target - m_lastGrabTarget) * (1.0 / frameDt);
        m_grabVelocity =
            m_grabVelocity * (1.0 - GRAB_VELOCITY_SMOOTHING) + instant * GRAB_VELOCITY_SMOOTHING;
    }
    m_lastGrabTarget = target;

    obj->setPosition(target);
    // While held, the object moves with the mouse so it pushes others realistically.
    obj->setVelocity(m_grabVelocity);
}

void Sandbox::validate(const physics::PhysicsWorld& world)
{
    if (m_selected && !world.find(*m_selected))
    {
        m_selected.reset();
        m_grabbing = false;
        m_grabPending = false;
    }
}

bool Sandbox::deleteSelected(physics::PhysicsWorld& world)
{
    if (!m_selected)
        return false;
    const bool removed = world.remove(*m_selected);
    m_selected.reset();
    m_grabbing = false;
    m_grabPending = false;
    return removed;
}

std::optional<object::ObjectId> Sandbox::duplicateSelected(physics::PhysicsWorld& world)
{
    const GameObject* original = m_selected ? world.find(*m_selected) : nullptr;
    if (!original)
        return std::nullopt;
    GameObject copy = *original;
    const double offset = original->boundingRadius() * 2.0 + 5.0;
    copy.setPosition(original->getPosition() + Vector2D{offset, 0.0});
    m_selected = world.add(std::move(copy));  // `original` may dangle after this
    return m_selected;
}

GameObject Sandbox::makeFromTemplate(const Vector2D& position, const Vector2D& velocity,
                                     std::mt19937& rng) const
{
    const int counter = ++m_spawnCounter;
    GameObject obj((spawn.circle ? "Circle" : "Rect") + std::to_string(counter), spawn.shape());
    obj.setPosition(position);
    obj.setMass(spawn.massFromArea ? object::area(obj.getShape()) : spawn.mass);
    obj.setRestitution(spawn.restitution);
    obj.setVelocity(velocity);
    obj.setStable(spawn.stable);
    if (spawn.randomColor)
        obj.setColor(object::colorFromHue(portable::uniform(rng, 0.0, 360.0)));
    else
        obj.setColor(spawn.color);
    return obj;
}

std::optional<DragLine> Sandbox::spawnDragLine() const
{
    if (!m_spawnStart)
        return std::nullopt;
    return DragLine{*m_spawnStart, m_mouse};
}

std::optional<object::ObjectId> Sandbox::hovered(const physics::PhysicsWorld& world) const
{
    return world.objectAt(m_mouse);
}
}  // namespace game::sandbox
