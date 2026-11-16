#include "Collision.hpp"

#include <algorithm>
#include <cmath>

namespace game::physics
{
using math::Vector2D;
using object::CircleShape;
using object::RectShape;

namespace
{
constexpr double EPSILON = 1e-9;

std::optional<Contact> circleCircle(const Vector2D& pa, double ra, const Vector2D& pb, double rb)
{
    const Vector2D d = pb - pa;
    const double radii = ra + rb;
    const double distSq = d.lengthSquared();
    if (distSq >= radii * radii)
        return std::nullopt;

    const double dist = std::sqrt(distSq);
    if (dist < EPSILON)
        return Contact{Vector2D{1.0, 0.0}, radii};  // Exactly concentric: pick any axis.
    return Contact{d / dist, radii - dist};
}

// Normal points from the circle towards the rectangle.
std::optional<Contact> circleRect(const Vector2D& circlePos, double radius, const Vector2D& rectPos,
                                  const Vector2D& half)
{
    const Vector2D d = circlePos - rectPos;
    const double absX = std::fabs(d.getX());
    const double absY = std::fabs(d.getY());
    const bool centerInside = absX <= half.getX() && absY <= half.getY();

    if (!centerInside)
    {
        const Vector2D closest{rectPos.getX() + std::clamp(d.getX(), -half.getX(), half.getX()),
                               rectPos.getY() + std::clamp(d.getY(), -half.getY(), half.getY())};
        const Vector2D toClosest = closest - circlePos;
        const double distSq = toClosest.lengthSquared();
        if (distSq >= radius * radius)
            return std::nullopt;
        const double dist = std::sqrt(distSq);
        if (dist < EPSILON)
            return Contact{Vector2D{1.0, 0.0}, radius};
        return Contact{toClosest / dist, radius - dist};
    }

    // Circle center is inside the rectangle: push out along the axis of least overlap.
    const double overlapX = half.getX() - absX;
    const double overlapY = half.getY() - absY;
    if (overlapX < overlapY)
    {
        const double sign = d.getX() > 0.0 ? -1.0 : 1.0;
        return Contact{Vector2D{sign, 0.0}, radius + overlapX};
    }
    const double sign = d.getY() > 0.0 ? -1.0 : 1.0;
    return Contact{Vector2D{0.0, sign}, radius + overlapY};
}

std::optional<Contact> rectRect(const Vector2D& pa, const Vector2D& ha, const Vector2D& pb,
                                const Vector2D& hb)
{
    const Vector2D d = pb - pa;
    const double overlapX = ha.getX() + hb.getX() - std::fabs(d.getX());
    const double overlapY = ha.getY() + hb.getY() - std::fabs(d.getY());
    if (overlapX <= 0.0 || overlapY <= 0.0)
        return std::nullopt;

    if (overlapX < overlapY)
        return Contact{Vector2D{d.getX() >= 0.0 ? 1.0 : -1.0, 0.0}, overlapX};
    return Contact{Vector2D{0.0, d.getY() >= 0.0 ? 1.0 : -1.0}, overlapY};
}

Vector2D halfOf(const RectShape& r)
{
    return {r.width / 2.0, r.height / 2.0};
}
}  // namespace

std::optional<Contact> collide(const object::GameObject& a, const object::GameObject& b)
{
    const Vector2D& pa = a.getPosition();
    const Vector2D& pb = b.getPosition();

    return std::visit(
        object::Overloaded{
            [&](const CircleShape& ca, const CircleShape& cb) -> std::optional<Contact>
            {
                return circleCircle(pa, ca.radius, pb, cb.radius);
            },
            [&](const CircleShape& ca, const RectShape& rb) -> std::optional<Contact>
            {
                return circleRect(pa, ca.radius, pb, halfOf(rb));
            },
            [&](const RectShape& ra, const CircleShape& cb) -> std::optional<Contact>
            {
                auto contact = circleRect(pb, cb.radius, pa, halfOf(ra));
                if (contact)
                    contact->normal = -contact->normal;  // Flip so it points from A to B.
                return contact;
            },
            [&](const RectShape& ra, const RectShape& rb) -> std::optional<Contact>
            {
                return rectRect(pa, halfOf(ra), pb, halfOf(rb));
            }},
        a.getShape(), b.getShape());
}

bool containsPoint(const object::GameObject& obj, const Vector2D& point)
{
    const Vector2D d = point - obj.getPosition();
    return std::visit(object::Overloaded{[&](const CircleShape& c)
                                         {
                                             return d.lengthSquared() <= c.radius * c.radius;
                                         },
                                         [&](const RectShape& r)
                                         {
                                             return std::fabs(d.getX()) <= r.width / 2.0 &&
                                                    std::fabs(d.getY()) <= r.height / 2.0;
                                         }},
                      obj.getShape());
}
}  // namespace game::physics
