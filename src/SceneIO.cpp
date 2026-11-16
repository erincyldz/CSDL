#include "SceneIO.hpp"

#include <cerrno>
#include <cstdlib>
#include <fstream>
#include <limits>
#include <sstream>
#include <stdexcept>
#include <vector>

namespace game::scene
{
using object::GameObject;

namespace
{
bool toDouble(const std::string& s, double& out)
{
    char* end = nullptr;
    errno = 0;
    out = std::strtod(s.c_str(), &end);
    return !s.empty() && *end == '\0' && errno == 0;
}

bool toColor(const std::string& s, object::Color& out)
{
    int r = 0, g = 0, b = 0;
    char c1 = 0, c2 = 0;
    std::istringstream in(s);
    if (!(in >> r >> c1 >> g >> c2 >> b) || c1 != ',' || c2 != ',' || r < 0 || r > 255 || g < 0 ||
        g > 255 || b < 0 || b > 255)
        return false;
    out = object::Color{static_cast<std::uint8_t>(r), static_cast<std::uint8_t>(g),
                        static_cast<std::uint8_t>(b), 255};
    return true;
}

std::string sanitizeName(std::string name)
{
    for (char& c : name)
        if (c == ' ' || c == '\t' || c == '=' || c == '#')
            c = '_';
    return name.empty() ? "Object" : name;
}

// Parses one non-empty line into an object. Throws std::runtime_error with a message.
GameObject parseLine(const std::string& line)
{
    std::istringstream in(line);
    std::string kind;
    in >> kind;
    if (kind != "circle" && kind != "rect")
        throw std::runtime_error("unknown object type '" + kind + "' (expected circle or rect)");

    std::string name = kind;
    double x = 0, y = 0, vx = 0, vy = 0, radius = 10, width = 20, height = 20;
    double mass = -1, restitution = 1, stable = 0;
    object::Color color{255, 0, 0, 255};

    std::string token;
    while (in >> token)
    {
        const auto eq = token.find('=');
        if (eq == std::string::npos)
            throw std::runtime_error("expected key=value, got '" + token + "'");
        const std::string key = token.substr(0, eq);
        const std::string value = token.substr(eq + 1);

        if (key == "name")
        {
            name = value;
            continue;
        }
        if (key == "color")
        {
            if (!toColor(value, color))
                throw std::runtime_error("invalid color '" + value + "' (expected r,g,b)");
            continue;
        }
        double* target = key == "x"             ? &x
                         : key == "y"           ? &y
                         : key == "vx"          ? &vx
                         : key == "vy"          ? &vy
                         : key == "radius"      ? &radius
                         : key == "width"       ? &width
                         : key == "height"      ? &height
                         : key == "mass"        ? &mass
                         : key == "restitution" ? &restitution
                         : key == "stable"      ? &stable
                                                : nullptr;
        if (!target)
            throw std::runtime_error("unknown key '" + key + "'");
        if (!toDouble(value, *target))
            throw std::runtime_error("invalid number '" + value + "' for '" + key + "'");
    }

    if (kind == "circle" && !(radius > 0))
        throw std::runtime_error("radius must be positive");
    if (kind == "rect" && !(width > 0 && height > 0))
        throw std::runtime_error("width and height must be positive");

    GameObject obj =
        kind == "circle" ? GameObject::circle(name, radius) : GameObject::rect(name, width, height);
    obj.setPosition({x, y});
    obj.setMass(mass < 0 ? object::area(obj.getShape()) : mass);  // throws if mass == 0
    obj.setRestitution(restitution);
    obj.setStable(stable != 0.0);
    obj.setVelocity({vx, vy});
    obj.setColor(color);
    return obj;
}
}  // namespace

bool save(const physics::PhysicsWorld& world, const std::string& path, std::string& error)
{
    std::ofstream out(path);
    if (!out)
    {
        error = "cannot write '" + path + "'";
        return false;
    }
    out.precision(std::numeric_limits<double>::max_digits10);
    out << "# CSDL scene (" << world.size() << " objects)\n";
    for (const auto& obj : world.objects())
    {
        const auto& p = obj.getPosition();
        const auto& v = obj.getVelocity();
        const auto c = obj.getColor();
        std::visit(object::Overloaded{[&](const object::CircleShape& s)
                                      {
                                          out << "circle radius=" << s.radius;
                                      },
                                      [&](const object::RectShape& s)
                                      {
                                          out << "rect width=" << s.width << " height=" << s.height;
                                      }},
                   obj.getShape());
        out << " name=" << sanitizeName(obj.getName()) << " x=" << p.getX() << " y=" << p.getY()
            << " vx=" << v.getX() << " vy=" << v.getY() << " mass=" << obj.getMass()
            << " restitution=" << obj.getRestitution() << " stable=" << (obj.isStable() ? 1 : 0)
            << " color=" << int(c.r) << ',' << int(c.g) << ',' << int(c.b) << '\n';
    }
    if (!out)
    {
        error = "write error on '" + path + "'";
        return false;
    }
    return true;
}

bool load(physics::PhysicsWorld& world, const std::string& path, std::string& error)
{
    std::ifstream in(path);
    if (!in)
    {
        error = "cannot open '" + path + "'";
        return false;
    }

    std::vector<GameObject> objects;
    std::string line;
    int lineNo = 0;
    while (std::getline(in, line))
    {
        ++lineNo;
        if (const auto hash = line.find('#'); hash != std::string::npos)
            line.erase(hash);
        if (line.find_first_not_of(" \t\r") == std::string::npos)
            continue;
        try
        {
            objects.push_back(parseLine(line));
        }
        catch (const std::exception& e)
        {
            error = path + ":" + std::to_string(lineNo) + ": " + e.what();
            return false;
        }
    }

    world.clear();
    for (auto& obj : objects)
        world.add(std::move(obj));
    return true;
}
}  // namespace game::scene
