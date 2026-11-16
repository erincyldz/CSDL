#pragma once
#include "GameObject.hpp"

#include <optional>

namespace game::physics
{
struct Contact
{
    math::Vector2D normal;     // Unit vector pointing from object A towards object B.
    double penetration = 0.0;  // Overlap depth along the normal (> 0).
};

// Narrow-phase test. Returns a contact if the two shapes overlap.
std::optional<Contact> collide(const object::GameObject& a, const object::GameObject& b);

// True if `point` lies inside (or on the border of) the object's shape.
bool containsPoint(const object::GameObject& obj, const math::Vector2D& point);
}  // namespace game::physics
