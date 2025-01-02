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
        std::cout << "SA \n";
        ++s_instanceCounter;
        m_color_state = helper::ObjectColor::RED;
        m_color = {255, 0, 0};
    };
    virtual ~GameObject()
    {
        --s_instanceCounter;
    }

    void update(float delta_time);

    bool collision_detection(GameObject& obj);

    void destroy();

    void update_color(float delta_time);  // DONE

    int get_object_count();

  protected:
    helper::ObjectType m_type;

    ClassLogger m_logger;

    Force m_force;

    Position m_pos;

    float m_mass;

    Velocity m_velocity;

    helper::ObjectColor m_color_state;

    Color m_color;

  private:
    static int s_instanceCounter;
};
}  // namespace game::object
#endif