#include <GameObject.hpp>
int game_object_count = 0;
namespace game::object
{
void GameObject::update_color(float delta_time)
{
    switch (this->m_color_state)
    {
        case ColorState::RED:
            this->m_color.b += COLOR_CHANGE_SCALAR * delta_time;
            if (this->m_color.b == 255)
            {
                this->m_color_state = ColorState::PINK;
                break;
            }
            if (this->m_color.b >= 255)
            {
                this->m_color.b = 255;
                this->m_color_state = ColorState::PINK;
                break;
            }
            break;
        case ColorState::PINK:
            this->m_color.r -= COLOR_CHANGE_SCALAR * delta_time;
            if (this->m_color.r == 0)
            {
                this->m_color_state = ColorState::BLUE;
                break;
            }
            if (this->m_color.r <= 0)
            {
                this->m_color.r = 0;
                this->m_color_state = ColorState::BLUE;
                break;
            }
            break;
        case ColorState::BLUE:
            this->m_color.g += COLOR_CHANGE_SCALAR * delta_time;
            if (this->m_color.g == 255)
            {
                this->m_color_state = ColorState::CYAN;
                break;
            }
            if (this->m_color.g >= 255)
            {
                this->m_color.g = 255;
                this->m_color_state = ColorState::CYAN;
                break;
            }
            break;
        case ColorState::CYAN:
            this->m_color.b -= COLOR_CHANGE_SCALAR * delta_time;
            if (this->m_color.b == 0)
            {
                this->m_color_state = ColorState::GREEN;
                break;
            }
            if (this->m_color.b >= 0)
            {
                this->m_color.b = 0;
                this->m_color_state = ColorState::GREEN;
                break;
            }
            break;
        case ColorState::GREEN:
            this->m_color.r += COLOR_CHANGE_SCALAR * delta_time;
            if (this->m_color.r == 255)
            {
                this->m_color_state = ColorState::YELLOW;
                break;
            }
            if (this->m_color.r >= 255)
            {
                this->m_color.r = 255;
                this->m_color_state = ColorState::YELLOW;
                break;
            }
            break;
        case ColorState::YELLOW:
            this->m_color.g -= COLOR_CHANGE_SCALAR * delta_time;
            if (this->m_color.g == 0)
            {
                this->m_color_state = ColorState::RED;
                break;
            }
            if (this->m_color.g <= 0)
            {
                this->m_color.g = 0;
                this->m_color_state = ColorState::RED;
                break;
            }
            break;

        default:
            break;
    }
}
void GameObject::setVelocity(Velocity velocity)
{
    m_velocity = velocity;
}
void GameObject::setPosition(Position pos)
{
    m_pos = pos;
}
ObjectType GameObject::get_type() const
{
    return m_type;
}
Color GameObject::get_color() const
{
    return m_color;
}
Position GameObject::getPosition() const
{
    return m_pos;
}
}  // namespace game::object