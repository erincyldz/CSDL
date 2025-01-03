#ifndef GAME_OBJECT_H
#define GAME_OBJECT_H

#include "ClassLogger.hpp"
#include "ObjectHelper.hpp"

#include <algorithm>
#include <cmath>
#include <memory>
#include <vector>
namespace game::object
{
using Force = helper::Vector2_t;
using Position = helper::Vector2_t;
using Velocity = helper::Vector2_t;
using Color = helper::Color_t;
using ColorState = helper::ObjectColor;
using ObjectType = helper::ObjectType;

// forward declaration
class CircleObject;
class RectObject;

class GameObject
{

  public:
    explicit GameObject(std::string& logger_name) : m_logger(logger_name)
    {
        m_color_state = helper::ObjectColor::RED;
        m_color = {255, 0, 0};
        m_mass = 1.0f;
        m_force = {0.0f, 0.0f};
        m_velocity = {0.1f, 0.1f};
        m_pos = {0.0f, 0.0f};
        m_restitution = 1.0f;
    };
    virtual ~GameObject() {}

    virtual void update(
        float delta_time, int screen_width, int screen_height,
        const std::vector<game::object::GameObject*>& other_objects) = 0;  // Pure virtual function

    virtual bool border_collision(int screen_width, int screen_height) = 0;
    bool is_colliding_with(const GameObject& other) const;

    void destroy();
    void update_color(float delta_time);  // DONE
    void setVelocity(Velocity velocity);
    void setPosition(Position pos);
    ObjectType get_type() const;
    Color get_color() const;
    Position getPosition() const;
    helper::ObjectType m_type;
    Position m_pos;

  protected:
    ClassLogger m_logger;
    Force m_force;
    float m_mass;
    float m_restitution;  //  Coefficient of restitution (bounciness) for elastic/inelastic
                          //  collisions. 1.0 for perfectly elastic, 0.0 for perfectly inelastic.
    Velocity m_velocity;
    ColorState m_color_state;
    Color m_color;
};
}  // namespace game::object
#endif