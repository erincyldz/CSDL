#include <CircleObject.hpp>
#define PI 3.14159265358979323846

namespace game::object
{

CircleObject::CircleObject(float rad, std::string& logger_name)
    : GameObject(logger_name), m_radius(rad)
{
    m_type = helper::ObjectType::CIRCLE;
    m_type = helper::ObjectType::CIRCLE;
    m_mass = 1.0;
    m_force = {0.0f, 0.0f};
    m_restitution = 1;
    m_color_state = helper::ObjectColor::RED;
    m_color = {255, 0, 0};
};

CircleObject::CircleObject(std::string& logger_name) : GameObject(logger_name), m_radius(0.0f)
{
    m_type = helper::ObjectType::CIRCLE;
    m_radius = 50;
    m_color = {255, 0, 0};
    m_mass = 1.0f;
    m_force = {0.0f, 0.0f};
    m_velocity = {0.0f, 0.0f};
    m_pos = {400.0f, 325.0f};
    m_restitution = 1;
    m_color_state = helper::ObjectColor::RED;
};

float CircleObject::getRadius() const
{
    return m_radius;
}

void CircleObject::setRadius(float rad)
{
    m_radius = rad;
}
Position CircleObject::getCenter() const
{
    return Position{m_pos};
}
void CircleObject::tellRadius()
{
    m_logger.info("Radius is : {}", this->m_radius);
}

void CircleObject::update(float delta_time, int screen_width, int screen_height)
{
    // update_color(delta_time);
    // m_pos += m_velocity * delta_time;
    update_physics(delta_time);
    border_collision(screen_width, screen_height);
}

bool CircleObject::border_collision(int screen_width, int screen_height)
{
#ifdef BORDER_COLLISION
    if (m_pos.getX() - m_radius < 0 || m_pos.getX() + m_radius > screen_width)
    {
        m_velocity.reverseX();  // Reverse x velocity
        m_pos.setX(std::clamp(m_pos.getX(), (double)m_radius,
                              screen_width - (double)m_radius));  // Keep within bounds
        return true;
    }

    if (m_pos.getY() - m_radius < 0 || m_pos.getY() + m_radius > screen_height)
    {
        m_velocity.reverseY();  // Reverse y velocity
        m_pos.setY(std::clamp(m_pos.getY(), (double)m_radius,
                              screen_height - (double)m_radius));  // Keep within bounds
        return true;
    }
#else
    if (m_pos.getX() + m_radius < 0)
    {
        m_pos.setX(screen_width);
        return true;
    }
    if (m_pos.getX() - m_radius > screen_width)
    {
        m_pos.setX(0);
        return true;
    }

    if (m_pos.getY() + m_radius < 0)
    {
        m_pos.setY(screen_height);
        return true;
    }
    if (m_pos.getY() - m_radius > screen_height)
    {
        m_pos.setY(0);
        return true;
    }
#endif
    return false;
}

}  // namespace game::object