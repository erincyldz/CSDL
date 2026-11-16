#include "PhysicsWorld.hpp"

#include <algorithm>
#include <cmath>

namespace game::physics
{
using math::Vector2D;
using object::GameObject;
using object::ObjectId;

namespace
{
std::uint64_t cellKey(std::int32_t cx, std::int32_t cy)
{
    return (static_cast<std::uint64_t>(static_cast<std::uint32_t>(cx)) << 32) |
           static_cast<std::uint32_t>(cy);
}
}  // namespace

PhysicsWorld::PhysicsWorld(const PhysicsSettings& settings) : m_settings(settings) {}

void PhysicsWorld::setBounds(double width, double height)
{
    m_width = width;
    m_height = height;
}

ObjectId PhysicsWorld::add(GameObject obj)
{
    obj.setId(m_nextId++);
    obj.clearTrail();
    m_objects.push_back(std::move(obj));
    return m_objects.back().getId();
}

bool PhysicsWorld::remove(ObjectId id)
{
    auto it = std::find_if(m_objects.begin(), m_objects.end(),
                           [id](const GameObject& o)
                           {
                               return o.getId() == id;
                           });
    if (it == m_objects.end())
        return false;
    m_objects.erase(it);
    return true;
}

GameObject* PhysicsWorld::find(ObjectId id)
{
    for (auto& obj : m_objects)
        if (obj.getId() == id)
            return &obj;
    return nullptr;
}

const GameObject* PhysicsWorld::find(ObjectId id) const
{
    return const_cast<PhysicsWorld*>(this)->find(id);
}

std::optional<ObjectId> PhysicsWorld::objectAt(const Vector2D& point) const
{
    for (auto it = m_objects.rbegin(); it != m_objects.rend(); ++it)
        if (containsPoint(*it, point))
            return it->getId();
    return std::nullopt;
}

PhysicsWorld::EnergyStats PhysicsWorld::computeEnergy() const
{
    EnergyStats stats;
    for (const auto& obj : m_objects)
    {
        if (obj.isStable())
            continue;
        stats.kinetic += 0.5 * obj.getMass() * obj.getVelocity().lengthSquared();
        stats.momentum += obj.getVelocity() * obj.getMass();
    }
    if (m_settings.gravity)
    {
        // Potential matching the softened force: U = -G m1 m2 / sqrt(d^2 + eps^2).
        const double G = m_settings.gravitationalConstant;
        const double soft2 = m_settings.softening * m_settings.softening;
        for (std::size_t i = 0; i < m_objects.size(); ++i)
            for (std::size_t j = i + 1; j < m_objects.size(); ++j)
            {
                const auto& a = m_objects[i];
                const auto& b = m_objects[j];
                const double r =
                    std::sqrt((b.getPosition() - a.getPosition()).lengthSquared() + soft2);
                if (r > 0.0)
                    stats.potential -= G * a.getMass() * b.getMass() / r;
            }
    }
    return stats;
}

void PhysicsWorld::clear()
{
    m_objects.clear();
    m_previousContacts.clear();
    m_activeContacts.clear();
    m_newContacts = 0;
    m_totalCollisions = 0;
}

void PhysicsWorld::step(double dt)
{
    if (m_settings.gravity)
        applyGravity();

    for (auto& obj : m_objects)
    {
        if (m_settings.colorCycle)
            obj.updateColorCycle(dt);
        obj.integrate(dt, m_settings.maxSpeed);
    }

    detectAndResolveCollisions();

    for (auto& obj : m_objects)
    {
        handleBorder(obj);
        obj.recordTrail();
    }
}

void PhysicsWorld::applyGravity()
{
    const double G = m_settings.gravitationalConstant;
    const double soft2 = m_settings.softening * m_settings.softening;

    // Each pair once; Newton's third law gives the opposite force for free.
    for (std::size_t i = 0; i < m_objects.size(); ++i)
    {
        for (std::size_t j = i + 1; j < m_objects.size(); ++j)
        {
            GameObject& a = m_objects[i];
            GameObject& b = m_objects[j];
            const Vector2D d = b.getPosition() - a.getPosition();
            const double distSq = d.lengthSquared() + soft2;
            if (distSq <= 0.0)
                continue;
            const double dist = std::sqrt(distSq);
            const double magnitude = G * a.getMass() * b.getMass() / distSq;
            const Vector2D force = d * (magnitude / dist);
            a.addForce(force);
            b.addForce(-force);
        }
    }
}

void PhysicsWorld::handleBorder(GameObject& obj) const
{
    if (obj.isStable())
        return;

    const Vector2D half = obj.halfExtents();
    Vector2D pos = obj.getPosition();

    if (!m_settings.borderCollision)
    {
        // Wrap around once the object has fully left the screen.
        if (pos.getX() + half.getX() < 0.0)
            pos.setX(m_width + half.getX());
        else if (pos.getX() - half.getX() > m_width)
            pos.setX(-half.getX());
        if (pos.getY() + half.getY() < 0.0)
            pos.setY(m_height + half.getY());
        else if (pos.getY() - half.getY() > m_height)
            pos.setY(-half.getY());
        obj.setPosition(pos);
        return;
    }

    Vector2D vel = obj.getVelocity();
    const double e = obj.getRestitution();
    // Both axes are checked every step so corner hits are handled correctly.
    if (pos.getX() - half.getX() < 0.0)
    {
        pos.setX(half.getX());
        if (vel.getX() < 0.0)
            vel.setX(-vel.getX() * e);
    }
    else if (pos.getX() + half.getX() > m_width)
    {
        pos.setX(std::max(half.getX(), m_width - half.getX()));
        if (vel.getX() > 0.0)
            vel.setX(-vel.getX() * e);
    }
    if (pos.getY() - half.getY() < 0.0)
    {
        pos.setY(half.getY());
        if (vel.getY() < 0.0)
            vel.setY(-vel.getY() * e);
    }
    else if (pos.getY() + half.getY() > m_height)
    {
        pos.setY(std::max(half.getY(), m_height - half.getY()));
        if (vel.getY() > 0.0)
            vel.setY(-vel.getY() * e);
    }
    obj.setPosition(pos);
    obj.setVelocity(vel);
}

void PhysicsWorld::detectAndResolveCollisions()
{
    m_activeContacts.clear();
    m_newContacts = 0;

    std::unordered_set<std::uint64_t> currentContacts;
    if (m_objects.size() < 2)
    {
        m_previousContacts.clear();
        return;
    }

    // Broad phase: uniform grid with cell size >= the largest object diameter. Two overlapping
    // objects are then always in the same or in adjacent cells.
    double cellSize = 1.0;
    for (const auto& obj : m_objects)
        cellSize = std::max(cellSize, 2.0 * obj.boundingRadius());

    for (auto& [key, bucket] : m_grid)
        bucket.clear();

    std::vector<std::pair<std::int32_t, std::int32_t>> cells(m_objects.size());
    for (std::size_t i = 0; i < m_objects.size(); ++i)
    {
        const Vector2D& p = m_objects[i].getPosition();
        const auto cx = static_cast<std::int32_t>(std::floor(p.getX() / cellSize));
        const auto cy = static_cast<std::int32_t>(std::floor(p.getY() / cellSize));
        cells[i] = {cx, cy};
        m_grid[cellKey(cx, cy)].push_back(i);
    }

    // Narrow phase + resolution. Each pair is visited once (j > i).
    for (std::size_t i = 0; i < m_objects.size(); ++i)
    {
        const auto [cx, cy] = cells[i];
        for (std::int32_t dx = -1; dx <= 1; ++dx)
        {
            for (std::int32_t dy = -1; dy <= 1; ++dy)
            {
                const auto it = m_grid.find(cellKey(cx + dx, cy + dy));
                if (it == m_grid.end())
                    continue;
                for (const std::size_t j : it->second)
                {
                    if (j <= i)
                        continue;
                    GameObject& a = m_objects[i];
                    GameObject& b = m_objects[j];
                    if (a.isStable() && b.isStable())
                        continue;  // e.g. overlapping wall corners: not a collision
                    const auto contact = collide(a, b);
                    if (!contact)
                        continue;

                    resolve(a, b, *contact);

                    const std::uint64_t key = pairKey(a.getId(), b.getId());
                    currentContacts.insert(key);
                    m_activeContacts.emplace_back(a.getId(), b.getId());
                    if (m_previousContacts.count(key) == 0)
                    {
                        ++m_newContacts;
                        ++m_totalCollisions;
                    }
                }
            }
        }
    }

    // Drop empty buckets occasionally so the map doesn't grow without bound.
    if (m_grid.size() > 4 * m_objects.size() + 64)
        m_grid.clear();

    m_previousContacts = std::move(currentContacts);
}

void PhysicsWorld::resolve(GameObject& a, GameObject& b, const Contact& contact) const
{
    const double invA = a.getInverseMass();
    const double invB = b.getInverseMass();
    const double invSum = invA + invB;
    if (invSum <= 0.0)
        return;  // Two stable objects.

    // Impulse along the contact normal (only if the objects are approaching).
    const Vector2D relativeVelocity = b.getVelocity() - a.getVelocity();
    const double closingSpeed = relativeVelocity.dot(contact.normal);
    if (closingSpeed < 0.0)
    {
        double e = std::min(a.getRestitution(), b.getRestitution());
        if (-closingSpeed < m_settings.restitutionThreshold)
            e = 0.0;  // Resting contact: don't keep bouncing forever.
        const double j = -(1.0 + e) * closingSpeed / invSum;
        const Vector2D impulse = contact.normal * j;
        a.applyImpulse(-impulse);
        b.applyImpulse(impulse);
    }

    // Positional correction so objects don't sink into each other.
    const double depth = std::max(contact.penetration - m_settings.correctionSlop, 0.0);
    if (depth > 0.0)
    {
        const Vector2D correction =
            contact.normal * (depth * m_settings.correctionPercent / invSum);
        a.translate(-correction * invA);
        b.translate(correction * invB);
    }
}

std::uint64_t PhysicsWorld::pairKey(ObjectId a, ObjectId b)
{
    if (a > b)
        std::swap(a, b);
    return (static_cast<std::uint64_t>(a) << 32) | b;
}
}  // namespace game::physics
