#include <RectObject.hpp>

namespace game::object
{
RectObject::RectObject(int width, int height, std::string& logger_name)
    : GameObject(logger_name), m_width(width), m_height(height)
{
    m_type = helper::ObjectType::RECTANGLE;
    m_mass = m_width * m_height;
    m_force = {0.0f, 0.0f};
    m_restitution = 1;
    m_color_state = helper::ObjectColor::RED;
    m_color = {255, 0, 0};
};

RectObject::RectObject(std::string& logger_name) : GameObject(logger_name)
{
    m_width = 50;
    m_height = 150;
    m_type = helper::ObjectType::RECTANGLE;
    m_color = {255, 0, 0};
    m_mass = 1.0f;
    m_force = {0.0f, 0.0f};
    m_velocity = {0.0f, 0.0f};
    m_pos = {350.0f, 250.0f};
    m_restitution = 1;
    m_color_state = helper::ObjectColor::RED;
}

std::pair<int, int> RectObject::getDimensions() const
{
    return std::pair<int, int>(m_width, m_height);
}

int RectObject::get_width() const
{
    return m_width;
}
int RectObject::get_height() const
{
    return m_height;
}

void RectObject::setDimensions(int width, int height)
{
    m_width = width;
    m_height = height;
}

void RectObject::tellRadius()
{
    m_logger.error("Width is : {} \t Height is : {}", this->m_width, this->m_height);
}

void RectObject::update(float delta_time, int screen_width, int screen_height)
{
    // update_color(delta_time);
    // m_pos.x += m_velocity.x * delta_time;
    // m_pos.y += m_velocity.y * delta_time;
    update_physics(delta_time);
    border_collision(screen_width, screen_height);
}

bool RectObject::border_collision(int screen_width, int screen_height)
{

    if (this->m_pos.x < 0)
    {
        this->m_pos.x = 0;
        this->m_velocity.x *= -1;
        return true;
    }
    if (this->m_pos.x + this->m_width > screen_width)
    {
        this->m_pos.x = screen_width - this->m_width;
        this->m_velocity.x *= -1;
        return true;
    }
    if (this->m_pos.y < 0)
    {
        this->m_pos.y = 0;
        this->m_velocity.y *= -1;
        return true;
    }
    if (this->m_pos.y + this->m_height > screen_height)
    {
        this->m_pos.y = screen_height - this->m_height;
        this->m_velocity.y *= -1;
        return true;
    }
    return false;
}

}  // namespace game::object