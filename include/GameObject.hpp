#pragma once
#include "Shape.hpp"
#include "Vector2D.hpp"

#include <array>
#include <cstddef>
#include <cstdint>
#include <string>

namespace game::object
{
using Position = math::Vector2D;
using Velocity = math::Vector2D;
using Force = math::Vector2D;

struct Color
{
    std::uint8_t r = 255;
    std::uint8_t g = 0;
    std::uint8_t b = 0;
    std::uint8_t a = 255;
};

// Converts a hue in degrees (saturation = value = 1) to an RGB color.
Color colorFromHue(double hueDegrees);

using ObjectId = std::uint32_t;

// A rigid body with a circle or rectangle shape. Position is the shape's center.
class GameObject
{
  public:
    static constexpr std::size_t TRAIL_LENGTH = 100;

    GameObject(std::string name, Shape shape);
    static GameObject circle(std::string name, double radius);
    static GameObject rect(std::string name, double width, double height);

    // Identity (the id is assigned by PhysicsWorld when the object is added).
    ObjectId getId() const
    {
        return m_id;
    }
    void setId(ObjectId id)
    {
        m_id = id;
    }
    const std::string& getName() const
    {
        return m_name;
    }

    // Shape
    const Shape& getShape() const
    {
        return m_shape;
    }
    void setShape(Shape shape)
    {
        m_shape = shape;
    }
    math::Vector2D halfExtents() const
    {
        return object::halfExtents(m_shape);
    }
    double boundingRadius() const
    {
        return object::boundingRadius(m_shape);
    }

    // Kinematics
    const Position& getPosition() const
    {
        return m_pos;
    }
    void setPosition(const Position& pos)
    {
        m_pos = pos;
    }
    const Velocity& getVelocity() const
    {
        return m_vel;
    }
    void setVelocity(const Velocity& vel);  // Ignored (kept at zero) for stable objects.
    const Force& getForce() const
    {
        return m_force;
    }
    void addForce(const Force& force)
    {
        m_force += force;
    }
    void applyImpulse(const math::Vector2D& impulse);  // v += J / m (no-op if stable)
    void translate(const math::Vector2D& delta);       // no-op if stable

    // Mass properties. Stable objects behave as if they had infinite mass.
    double getMass() const
    {
        return m_mass;
    }
    double getInverseMass() const
    {
        return m_stable ? 0.0 : 1.0 / m_mass;
    }
    void setMass(double mass);  // Throws std::invalid_argument for mass <= 0.
    double getRestitution() const
    {
        return m_restitution;
    }
    void setRestitution(double restitution);  // Clamped to [0, 1].
    bool isStable() const
    {
        return m_stable;
    }
    void setStable(bool stable);

    // Semi-implicit Euler step. Consumes the accumulated force.
    void integrate(double dt, double maxSpeed);

    // Visuals
    Color getColor() const
    {
        return m_color;
    }
    void setColor(Color color)
    {
        m_color = color;
    }
    void updateColorCycle(double dt);
    void recordTrail();
    void clearTrail();
    template<typename F>
    void forEachTrailPoint(F&& fn) const
    {
        for (std::size_t i = 0; i < m_trailCount; ++i)
            fn(m_trail[(m_trailHead + i) % TRAIL_LENGTH]);
    }

  private:
    ObjectId m_id = 0;
    std::string m_name;
    Shape m_shape;

    Position m_pos;
    Velocity m_vel;
    Force m_force;
    double m_mass = 1.0;
    double m_restitution = 1.0;  // 1 = perfectly elastic, 0 = perfectly inelastic
    bool m_stable = false;

    Color m_color;
    double m_hue = 0.0;

    std::array<Position, TRAIL_LENGTH> m_trail{};
    std::size_t m_trailHead = 0;
    std::size_t m_trailCount = 0;
};
}  // namespace game::object
