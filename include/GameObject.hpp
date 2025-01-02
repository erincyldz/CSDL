#ifndef GAME_OBJECT_H
#define GAME_OBJECT_H

#include "ClassLogger.hpp"
#include "ObjectHelper.hpp"

namespace game::object
{
using Force = helper::Vector2_t;
using Position = helper::Vector2_t;
using Velocity = helper::Vector2_t;
using Color = helper::Color_t;
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
    };
    virtual ~GameObject() {}
    virtual void update(float delta_time) = 0;  // Pure virtual function
    bool collision_detection(GameObject& obj);
    void destroy();
    void update_color(float delta_time);  // DONE
    helper::ObjectType get_type() const;
    Color get_color() const
    {
        return m_color;
    }

  protected:
    helper::ObjectType m_type;
    ClassLogger m_logger;
    Force m_force;
    Position m_pos;
    float m_mass;
    Velocity m_velocity;
    helper::ObjectColor m_color_state;
    Color m_color;
};
}  // namespace game::object
#endif