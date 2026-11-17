#pragma once
#include "Vector2D.hpp"

#include <cmath>
#include <variant>

namespace game::object
{
struct CircleShape
{
    double radius = 10.0;
};

struct RectShape
{
    double width = 10.0;
    double height = 10.0;
};

// Every object's position is the CENTER of its shape, for both circles and rectangles.
using Shape = std::variant<CircleShape, RectShape>;

// Helper for std::visit with a set of lambdas.
template<class... Ts>
struct Overloaded : Ts...
{
    using Ts::operator()...;
};
template<class... Ts>
Overloaded(Ts...) -> Overloaded<Ts...>;

inline math::Vector2D halfExtents(const Shape& shape)
{
    return std::visit(Overloaded{[](const CircleShape& c)
                                 {
                                     return math::Vector2D{c.radius, c.radius};
                                 },
                                 [](const RectShape& r)
                                 {
                                     return math::Vector2D{r.width / 2.0, r.height / 2.0};
                                 }},
                      shape);
}

// Radius of the smallest circle around the center that contains the shape.
inline double boundingRadius(const Shape& shape)
{
    return std::visit(Overloaded{[](const CircleShape& c)
                                 {
                                     return c.radius;
                                 },
                                 [](const RectShape& r)
                                 {
                                     // Not std::hypot: its result differs between C runtimes,
                                     // and this sets the collision grid (and so the order in
                                     // which contacts are resolved). sqrt is exact everywhere.
                                     const double hw = r.width / 2.0;
                                     const double hh = r.height / 2.0;
                                     return std::sqrt(hw * hw + hh * hh);
                                 }},
                      shape);
}

inline double area(const Shape& shape)
{
    return std::visit(Overloaded{[](const CircleShape& c)
                                 {
                                     return math::PI * c.radius * c.radius;
                                 },
                                 [](const RectShape& r)
                                 {
                                     return r.width * r.height;
                                 }},
                      shape);
}
}  // namespace game::object
