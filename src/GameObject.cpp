#include <GameObject.hpp>
int game_object_count = 0;
namespace game::object
{
void GameObject::update_color(float delta_time)
{
    switch (this->m_color_state)
    {
        case helper::ObjectColor::RED:
            this->m_color.b += COLOR_CHANGE_SCALAR * delta_time;
            if (this->m_color.b == 255)
            {
                this->m_color_state = helper::ObjectColor::PINK;
                break;
            }
            if (this->m_color.b >= 255)
            {
                this->m_color.b = 255;
                this->m_color_state = helper::ObjectColor::PINK;
                break;
            }
            break;
        case helper::ObjectColor::PINK:
            this->m_color.r -= COLOR_CHANGE_SCALAR * delta_time;
            if (this->m_color.r == 0)
            {
                this->m_color_state = helper::ObjectColor::BLUE;
                break;
            }
            if (this->m_color.r <= 0)
            {
                this->m_color.r = 0;
                this->m_color_state = helper::ObjectColor::BLUE;
                break;
            }
            break;
        case helper::ObjectColor::BLUE:
            this->m_color.g += COLOR_CHANGE_SCALAR * delta_time;
            if (this->m_color.g == 255)
            {
                this->m_color_state = helper::ObjectColor::CYAN;
                break;
            }
            if (this->m_color.g >= 255)
            {
                this->m_color.g = 255;
                this->m_color_state = helper::ObjectColor::CYAN;
                break;
            }
            break;
        case helper::ObjectColor::CYAN:
            this->m_color.b -= COLOR_CHANGE_SCALAR * delta_time;
            if (this->m_color.b == 0)
            {
                this->m_color_state = helper::ObjectColor::GREEN;
                break;
            }
            if (this->m_color.b >= 0)
            {
                this->m_color.b = 0;
                this->m_color_state = helper::ObjectColor::GREEN;
                break;
            }
            break;
        case helper::ObjectColor::GREEN:
            this->m_color.r += COLOR_CHANGE_SCALAR * delta_time;
            if (this->m_color.r == 255)
            {
                this->m_color_state = helper::ObjectColor::YELLOW;
                break;
            }
            if (this->m_color.r >= 255)
            {
                this->m_color.r = 255;
                this->m_color_state = helper::ObjectColor::YELLOW;
                break;
            }
            break;
        case helper::ObjectColor::YELLOW:
            this->m_color.g -= COLOR_CHANGE_SCALAR * delta_time;
            if (this->m_color.g == 0)
            {
                this->m_color_state = helper::ObjectColor::RED;
                break;
            }
            if (this->m_color.g <= 0)
            {
                this->m_color.g = 0;
                this->m_color_state = helper::ObjectColor::RED;
                break;
            }
            break;

        default:
            break;
    }
}

helper::ObjectType GameObject::get_type() const
{
    return m_type;
}

}  // namespace game::object