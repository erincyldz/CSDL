#pragma once
#include "ClassLogger.hpp"
#include "Collision.hpp"
#include "GameObject.hpp"

#include <cstddef>
#include <cstdint>
#include <optional>
#include <unordered_map>
#include <unordered_set>
#include <utility>
#include <vector>

namespace game::physics
{
struct PhysicsSettings
{
    bool gravity = true;
    double gravitationalConstant = 100.0;
    double softening = 5.0;       // Plummer softening length, avoids infinite force at d -> 0.
    bool borderCollision = true;  // false: objects wrap around the window edges.
    double maxSpeed = 500.0;      // Velocity magnitude clamp (<= 0 disables it).
    double restitutionThreshold = 20.0;  // Closing speeds below this are treated as inelastic.
    double correctionPercent = 0.8;      // Fraction of penetration removed per step.
    double correctionSlop = 0.5;         // Penetration (px) tolerated without correction.
    bool colorCycle = false;
};

class PhysicsWorld
{
  public:
    using ContactPair = std::pair<object::ObjectId, object::ObjectId>;

    explicit PhysicsWorld(const PhysicsSettings& settings = {});

    const PhysicsSettings& settings() const
    {
        return m_settings;
    }
    void setSettings(const PhysicsSettings& settings)
    {
        m_settings = settings;
    }
    void setBounds(double width, double height);
    double width() const
    {
        return m_width;
    }
    double height() const
    {
        return m_height;
    }

    object::ObjectId add(object::GameObject obj);
    bool remove(object::ObjectId id);
    void clear();  // Removes all objects and resets collision statistics.
    std::size_t size() const
    {
        return m_objects.size();
    }
    const std::vector<object::GameObject>& objects() const
    {
        return m_objects;
    }
    std::vector<object::GameObject>& objects()
    {
        return m_objects;
    }
    object::GameObject* find(object::ObjectId id);
    const object::GameObject* find(object::ObjectId id) const;
    // Topmost (last drawn) object containing the point.
    std::optional<object::ObjectId> objectAt(const math::Vector2D& point) const;

    void step(double dt);

    struct EnergyStats
    {
        double kinetic = 0.0;
        double potential = 0.0;   // Softened gravitational potential (0 when gravity is off).
        math::Vector2D momentum;  // Sum over movable objects.
        double total() const
        {
            return kinetic + potential;
        }
    };
    EnergyStats computeEnergy() const;

    const std::vector<ContactPair>& activeContacts() const
    {
        return m_activeContacts;
    }
    std::size_t newContactsLastStep() const
    {
        return m_newContacts;
    }
    std::uint64_t totalCollisions() const
    {
        return m_totalCollisions;
    }

  private:
    void applyGravity();
    void handleBorder(object::GameObject& obj) const;
    void detectAndResolveCollisions();
    void resolve(object::GameObject& a, object::GameObject& b, const Contact& contact) const;
    static std::uint64_t pairKey(object::ObjectId a, object::ObjectId b);

    PhysicsSettings m_settings;
    ClassLogger m_logger{"PhysicsWorld"};
    double m_width = 600.0;
    double m_height = 600.0;

    std::vector<object::GameObject> m_objects;
    object::ObjectId m_nextId = 1;

    std::unordered_set<std::uint64_t> m_previousContacts;
    std::vector<ContactPair> m_activeContacts;
    std::size_t m_newContacts = 0;
    std::uint64_t m_totalCollisions = 0;

    // Broad-phase scratch storage (kept between steps to avoid reallocations).
    std::unordered_map<std::uint64_t, std::vector<std::size_t>> m_grid;
};
}  // namespace game::physics
