#include "Scenario.hpp"

#include <algorithm>
#include <cmath>
#include <string>

namespace game::scenario
{
using object::GameObject;
using object::Position;
using object::Velocity;

void loadDefault(physics::PhysicsWorld& world)
{
    const double w = world.width();
    const double h = world.height();

    GameObject sun = GameObject::circle("Circle1", 30.0);
    sun.setPosition({w / 2.0, h / 2.0});
    sun.setMass(450.0);
    sun.setRestitution(0.5);
    sun.setStable(true);
    world.add(std::move(sun));

    GameObject planet = GameObject::circle("Circle2", 10.0);
    planet.setPosition({w / 2.0 - 250.0, h / 2.0 - 250.0});
    planet.setMass(15.0);
    planet.setRestitution(1.0);
    planet.setVelocity({30.0, 5.0});
    world.add(std::move(planet));
}

GameObject makeRandomObject(std::mt19937& rng, double worldWidth, double worldHeight)
{
    static int counter = 0;
    std::uniform_real_distribution<double> radius(10.0, 30.0);
    std::uniform_real_distribution<double> side(10.0, 50.0);
    std::uniform_real_distribution<double> speed(-100.0, 100.0);
    std::uniform_real_distribution<double> hue(0.0, 360.0);
    std::bernoulli_distribution isCircle(0.5);

    GameObject obj =
        isCircle(rng)
            ? GameObject::circle("RandomCircle" + std::to_string(++counter), radius(rng))
            : GameObject::rect("RandomRect" + std::to_string(++counter), side(rng), side(rng));

    const auto half = obj.halfExtents();
    const double maxX = std::max(half.getX(), worldWidth - half.getX());
    const double maxY = std::max(half.getY(), worldHeight - half.getY());
    std::uniform_real_distribution<double> x(half.getX(), maxX);
    std::uniform_real_distribution<double> y(half.getY(), maxY);

    obj.setPosition(Position{x(rng), y(rng)});
    obj.setMass(object::area(obj.getShape()));
    obj.setVelocity(Velocity{speed(rng), speed(rng)});
    obj.setColor(object::colorFromHue(hue(rng)));
    return obj;
}

void loadRandom(physics::PhysicsWorld& world, std::mt19937& rng, int count)
{
    for (int i = 0; i < count; ++i)
        world.add(makeRandomObject(rng, world.width(), world.height()));
}

namespace
{
GameObject ball(const std::string& name, double radius, double mass, Position pos, Velocity vel,
                object::Color color, double restitution = 1.0)
{
    GameObject obj = GameObject::circle(name, radius);
    obj.setPosition(pos);
    obj.setMass(mass);
    obj.setRestitution(restitution);
    obj.setVelocity(vel);
    obj.setColor(color);
    return obj;
}

GameObject wall(const std::string& name, Position center, double width, double height)
{
    GameObject obj = GameObject::rect(name, width, height);
    obj.setPosition(center);
    obj.setMass(1.0);  // Stable objects act as infinite mass in collisions; keep gravity small.
    obj.setRestitution(1.0);
    obj.setStable(true);
    obj.setColor({120, 120, 120, 255});
    return obj;
}

void loadCircularOrbit(physics::PhysicsWorld& world)
{
    const auto& s = world.settings();
    const double cx = world.width() / 2.0;
    const double cy = world.height() / 2.0;
    const double sunMass = 1000.0;

    GameObject sun = ball("Sun", 25.0, sunMass, {cx, cy}, {}, {255, 200, 0, 255}, 0.5);
    sun.setStable(true);
    world.add(std::move(sun));

    const double maxRadius = std::min(cx, cy) * 0.85;
    const object::Color colors[] = {{0, 180, 255, 255}, {0, 255, 120, 255}, {255, 80, 80, 255}};
    for (int i = 0; i < 3; ++i)
    {
        const double r = maxRadius * (0.45 + 0.27 * i);
        // Circular speed for the softened force: a = G M r / (r^2 + eps^2)^(3/2), v = sqrt(a r).
        const double eps2 = s.softening * s.softening;
        const double accel = s.gravitationalConstant * sunMass * r / std::pow(r * r + eps2, 1.5);
        const double v = s.gravity ? std::sqrt(accel * r) : 0.0;
        // Alternate the starting side so the planets don't line up.
        const double sign = (i % 2 == 0) ? 1.0 : -1.0;
        world.add(ball("Planet" + std::to_string(i + 1), 6.0, 1.0, {cx + sign * r, cy},
                       {0.0, -sign * v}, colors[i]));
    }
}

void loadHeadOn(physics::PhysicsWorld& world)
{
    const double cx = world.width() / 2.0;
    const double h = world.height();
    // Equal masses: velocities swap.
    world.add(ball("EqualA", 20.0, 10.0, {cx - 200.0, h * 0.25}, {100.0, 0.0}, {0, 180, 255, 255}));
    world.add(
        ball("EqualB", 20.0, 10.0, {cx + 200.0, h * 0.25}, {-100.0, 0.0}, {255, 120, 0, 255}));
    // Heavy vs light: the heavy one barely slows down.
    world.add(ball("Heavy", 30.0, 40.0, {cx - 200.0, h * 0.5}, {80.0, 0.0}, {200, 0, 200, 255}));
    world.add(ball("Light", 12.0, 5.0, {cx + 100.0, h * 0.5}, {0.0, 0.0}, {0, 255, 120, 255}));
    // Equal rectangles.
    for (int i = 0; i < 2; ++i)
    {
        GameObject r = GameObject::rect(i == 0 ? "RectA" : "RectB", 40.0, 40.0);
        r.setPosition({cx + (i == 0 ? -200.0 : 200.0), h * 0.75});
        r.setMass(10.0);
        r.setVelocity({i == 0 ? 100.0 : -100.0, 0.0});
        r.setColor(i == 0 ? object::Color{0, 180, 255, 255} : object::Color{255, 120, 0, 255});
        world.add(std::move(r));
    }
}

void loadNewtonsCradle(physics::PhysicsWorld& world)
{
    const double cx = world.width() / 2.0;
    const double cy = world.height() / 2.0;
    constexpr double R = 15.0;
    constexpr double GAP = 1.0;  // Tiny gap so the balls start without contact
    for (int i = 0; i < 5; ++i)
        world.add(ball("Ball" + std::to_string(i + 1), R, 5.0, {cx + i * (2 * R + GAP), cy}, {},
                       {180, 180, 180, 255}));
    world.add(ball("Striker", R, 5.0, {cx - 200.0, cy}, {150.0, 0.0}, {255, 60, 60, 255}));
}

void loadContainer(physics::PhysicsWorld& world, std::mt19937& rng)
{
    const double cx = world.width() / 2.0;
    const double cy = world.height() / 2.0;
    const double size = std::min(world.width(), world.height()) * 0.7;
    const double t = 16.0;  // wall thickness
    world.add(wall("WallTop", {cx, cy - size / 2}, size, t));
    world.add(wall("WallBottom", {cx, cy + size / 2}, size, t));
    world.add(wall("WallLeft", {cx - size / 2, cy}, t, size));
    world.add(wall("WallRight", {cx + size / 2, cy}, t, size));

    std::uniform_real_distribution<double> pos(-size / 2 + 40.0, size / 2 - 40.0);
    std::uniform_real_distribution<double> speed(-150.0, 150.0);
    std::uniform_real_distribution<double> hue(0.0, 360.0);
    for (int i = 0; i < 12; ++i)
        world.add(ball("Ball" + std::to_string(i + 1), 10.0, 5.0, {cx + pos(rng), cy + pos(rng)},
                       {speed(rng), speed(rng)}, object::colorFromHue(hue(rng))));
}
}  // namespace

const char* presetName(Preset preset)
{
    switch (preset)
    {
        case Preset::Default:
            return "Default";
        case Preset::Empty:
            return "Empty";
        case Preset::Random:
            return "Random";
        case Preset::CircularOrbit:
            return "Circular orbits";
        case Preset::HeadOn:
            return "Head-on collisions";
        case Preset::NewtonsCradle:
            return "Newton's cradle";
        case Preset::Container:
            return "Container";
    }
    return "?";
}

const char* presetKey(Preset preset)
{
    switch (preset)
    {
        case Preset::Default:
            return "default";
        case Preset::Empty:
            return "empty";
        case Preset::Random:
            return "random";
        case Preset::CircularOrbit:
            return "orbits";
        case Preset::HeadOn:
            return "head_on";
        case Preset::NewtonsCradle:
            return "cradle";
        case Preset::Container:
            return "container";
    }
    return "?";
}

std::optional<Preset> presetFromKey(std::string_view key)
{
    std::string normalized(key);
    std::replace(normalized.begin(), normalized.end(), '-', '_');
    for (int i = 0; i < PRESET_COUNT; ++i)
    {
        const auto preset = static_cast<Preset>(i);
        if (normalized == presetKey(preset))
            return preset;
    }
    return std::nullopt;
}

std::string presetKeyList()
{
    std::string list;
    for (int i = 0; i < PRESET_COUNT; ++i)
    {
        if (i > 0)
            list += ", ";
        list += presetKey(static_cast<Preset>(i));
    }
    return list;
}

const char* presetDescription(Preset preset)
{
    switch (preset)
    {
        case Preset::Default:
            return "A small body near a heavy stable one (the original setup).";
        case Preset::Empty:
            return "An empty world. Use the Spawn tool to build a scene.";
        case Preset::Random:
            return "Random circles and rectangles, mass = area.";
        case Preset::CircularOrbit:
            return "Planets launched at circular orbit speed (uses the current G and softening). "
                   "Orbits should stay round and total energy flat.";
        case Preset::HeadOn:
            return "Equal masses swap velocities; heavy vs light; equal rectangles. Best with "
                   "gravity off.";
        case Preset::NewtonsCradle:
            return "Momentum should pass through the row to the last ball. Best with gravity off.";
        case Preset::Container:
            return "Stable walls form a box; balls bounce inside. Energy should stay constant "
                   "with restitution 1.";
    }
    return "";
}

void loadPreset(physics::PhysicsWorld& world, Preset preset, std::mt19937& rng, int randomCount)
{
    world.clear();
    switch (preset)
    {
        case Preset::Default:
            loadDefault(world);
            break;
        case Preset::Empty:
            break;
        case Preset::Random:
            loadRandom(world, rng, randomCount);
            break;
        case Preset::CircularOrbit:
            loadCircularOrbit(world);
            break;
        case Preset::HeadOn:
            loadHeadOn(world);
            break;
        case Preset::NewtonsCradle:
            loadNewtonsCradle(world);
            break;
        case Preset::Container:
            loadContainer(world, rng);
            break;
    }
}
}  // namespace game::scenario
