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

void CircleObject::update(float delta_time, int screen_width, int screen_height)
{
    update_color(delta_time);
    m_pos.x += m_velocity.x * delta_time;
    m_pos.y += m_velocity.y * delta_time;
    collision_detection(screen_width, screen_height);
}
bool CircleObject::collision_detection(int screen_width, int screen_height)
{
    if (this->m_pos.x - m_radius < 0)
    {
        this->m_pos.x = m_radius;
        this->m_velocity.x *= -1;
    }
    if (this->m_pos.x + this->m_radius > screen_width)
    {
        this->m_pos.x = screen_width - this->m_radius;
        this->m_velocity.x *= -1;
    }
    if (this->m_pos.y - m_radius < 0)
    {
        this->m_pos.y = m_radius;
        this->m_velocity.y *= -1;
    }
    if (this->m_pos.y + this->m_radius > screen_height)
    {
        this->m_pos.y = screen_height - this->m_radius;
        this->m_velocity.y *= -1;
    }
}
}  // namespace game::object