#include "GameObject.hpp"

#include <algorithm>
#include <cmath>
#include <stdexcept>
#include <utility>

namespace game::object
{
namespace
{
constexpr double COLOR_CYCLE_DEGREES_PER_SECOND = 120.0;
}

Color colorFromHue(double hueDegrees)
{
    const double h = std::fmod(std::fmod(hueDegrees, 360.0) + 360.0, 360.0) / 60.0;
    const double x = 1.0 - std::fabs(std::fmod(h, 2.0) - 1.0);
    double r = 0.0, g = 0.0, b = 0.0;
    switch (static_cast<int>(h))
    {
        case 0:
            r = 1.0, g = x;
            break;
        case 1:
            r = x, g = 1.0;
            break;
        case 2:
            g = 1.0, b = x;
            break;
        case 3:
            g = x, b = 1.0;
            break;
        case 4:
            r = x, b = 1.0;
            break;
        default:
            r = 1.0, b = x;
            break;
    }
    auto to8 = [](double v)
    {
        return static_cast<std::uint8_t>(std::lround(v * 255.0));
    };
    return Color{to8(r), to8(g), to8(b), 255};
}

GameObject::GameObject(std::string name, Shape shape) : m_name(std::move(name)), m_shape(shape) {}

GameObject GameObject::circle(std::string name, double radius)
{
    return GameObject(std::move(name), CircleShape{radius});
}

GameObject GameObject::rect(std::string name, double width, double height)
{
    return GameObject(std::move(name), RectShape{width, height});
}

void GameObject::setVelocity(const Velocity& vel)
{
    m_vel = m_stable ? Velocity{} : vel;
}

void GameObject::applyImpulse(const math::Vector2D& impulse)
{
    if (!m_stable)
        m_vel += impulse * (1.0 / m_mass);
}

void GameObject::translate(const math::Vector2D& delta)
{
    if (!m_stable)
        m_pos += delta;
}

void GameObject::setMass(double mass)
{
    if (!(mass > 0.0))
        throw std::invalid_argument("GameObject '" + m_name + "': mass must be positive");
    m_mass = mass;
}

void GameObject::setRestitution(double restitution)
{
    m_restitution = std::clamp(restitution, 0.0, 1.0);
}

void GameObject::setStable(bool stable)
{
    m_stable = stable;
    if (stable)
        m_vel = {};
}

void GameObject::integrate(double dt, double maxSpeed)
{
    if (m_stable)
    {
        m_vel = {};
        m_force = {};
        return;
    }

    m_vel += m_force * (dt / m_mass);
    const double speed = m_vel.magnitude();
    if (maxSpeed > 0.0 && speed > maxSpeed)
        m_vel *= maxSpeed / speed;  // Clamp magnitude, keep direction.
    m_pos += m_vel * dt;
    m_force = {};
}

void GameObject::updateColorCycle(double dt)
{
    m_hue = std::fmod(m_hue + COLOR_CYCLE_DEGREES_PER_SECOND * dt, 360.0);
    const Color c = colorFromHue(m_hue);
    m_color = Color{c.r, c.g, c.b, m_color.a};
}

void GameObject::recordTrail()
{
    const std::size_t writeIdx = (m_trailHead + m_trailCount) % TRAIL_LENGTH;
    m_trail[writeIdx] = m_pos;
    if (m_trailCount < TRAIL_LENGTH)
        ++m_trailCount;
    else
        m_trailHead = (m_trailHead + 1) % TRAIL_LENGTH;
}

void GameObject::clearTrail()
{
    m_trailHead = 0;
    m_trailCount = 0;
}
}  // namespace game::object
