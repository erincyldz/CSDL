#include <RectObject.hpp>

namespace game::object
{
RectObject::RectObject(int width, int height, std::string& logger_name)
    : GameObject(logger_name), m_width(width), m_height(height)
{
    m_type = helper::ObjectType::RECTANGLE;
};

std::pair<int, int> RectObject::getDimensions() const
{
    return std::pair<int, int>(m_width, m_height);
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
    update_color(delta_time);
    m_pos.x += m_velocity.x * delta_time;
    m_pos.y += m_velocity.y * delta_time;
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