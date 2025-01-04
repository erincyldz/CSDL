#include <CircleObject.hpp>

namespace game::object
{

CircleObject::CircleObject(float rad, std::string& logger_name)
    : GameObject(logger_name), m_radius(rad)
{
    m_type = helper::ObjectType::CIRCLE;
};

float CircleObject::getRadius()
{
    return m_radius;
}

void CircleObject::setRadius(float rad)
{
    m_radius = rad;
}

void CircleObject::tellRadius()
{
    m_logger.info("Radius is : {}", this->m_radius);
}

void CircleObject::update(float delta_time, int screen_width, int screen_height,
                          const std::vector<game::object::GameObject*>& other_objects)
{
    update_color(delta_time);
    m_pos.x += m_velocity.x * delta_time;
    m_pos.y += m_velocity.y * delta_time;
    border_collision(screen_width, screen_height);

    for (auto& obj : other_objects)
    {
        if (is_colliding_with(*obj))
        {
            on_collision(*obj);
        }
    }
}

bool CircleObject::border_collision(int screen_width, int screen_height)
{
    if (m_pos.x - m_radius < 0 || m_pos.x + m_radius > screen_width)
    {
        m_velocity.x = -m_velocity.x;                                      // Reverse x velocity
        m_pos.x = std::clamp(m_pos.x, m_radius, screen_width - m_radius);  // Keep within bounds
        return true;
    }

    if (m_pos.y - m_radius < 0 || m_pos.y + m_radius > screen_height)
    {
        m_velocity.y = -m_velocity.y;                                       // Reverse y velocity
        m_pos.y = std::clamp(m_pos.y, m_radius, screen_height - m_radius);  // Keep within bounds
        return true;
    }
    return false;
}

}  // namespace game::object