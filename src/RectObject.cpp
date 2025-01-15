#include <RectObject.hpp>

namespace game::object
{
RectObject::RectObject(int width, int height, std::string& logger_name)
    : GameObject(logger_name), m_width(width), m_height(height)
{
    m_type = helper::ObjectType::RECTANGLE;
    m_mass = 1.0;
    m_force = {0.0f, 0.0f};
    m_restitution = 1;
    m_color_state = helper::ObjectColor::RED;
    m_color = {255, 0, 0};
};

RectObject::RectObject(std::string& logger_name) : GameObject(logger_name)
{
    m_width = 15;
    m_height = 15;
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
Position RectObject::getCenter() const
{
    auto center_x = m_pos.getX() + m_width / 2;
    auto center_y = m_pos.getY() + m_height / 2;
    return Position{center_x, center_y};
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
    // m_pos += m_velocity * delta_time;
    update_physics(delta_time);
    border_collision(screen_width, screen_height);
}

bool RectObject::border_collision(int screen_width, int screen_height)
{

    if (this->m_pos.getX() < 0)
    {
        this->m_pos.setX(0);
        this->m_velocity.reverseX();
        return true;
    }
    if (this->m_pos.getX() + this->m_width > screen_width)
    {
        this->m_pos.setX(screen_width - this->m_width);
        this->m_velocity.reverseX();
        return true;
    }
    if (this->m_pos.getY() < 0)
    {
        this->m_pos.setY(0);
        this->m_velocity.reverseY();
        return true;
    }
    if (this->m_pos.getY() + this->m_height > screen_height)
    {
        this->m_pos.setY(screen_height - this->m_height);
        this->m_velocity.reverseY();
        return true;
    }
    return false;
}

}  // namespace game::object