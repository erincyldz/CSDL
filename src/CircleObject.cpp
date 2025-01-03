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
    collides_with(std::move(other_objects));
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

bool CircleObject::collides_with(const std::vector<game::object::GameObject*>& other_objects)
{
    bool is_collision = false;

    for (auto& obj : other_objects)
    {
        if (obj->m_type == game::object::ObjectType::RECTANGLE)
        {
            game::object::RectObject* rect = dynamic_cast<game::object::RectObject*>(obj);
            auto closestX =
                std::max(rect->m_pos.x,
                         std::min(this->m_pos.x, rect->m_pos.x + rect->getDimensions().first));
            auto closestY =
                std::max(rect->m_pos.y,
                         std::min(this->m_pos.y, rect->m_pos.y + rect->getDimensions().second));
            auto distanceSquared = (this->m_pos.x - closestX) * (this->m_pos.x - closestX) +
                                   (this->m_pos.y - closestY) * (this->m_pos.y - closestY);
            if (distanceSquared <= this->m_radius * this->m_radius)
            {
                if (closestY == rect->m_pos.y ||
                    closestY == rect->m_pos.y + rect->getDimensions().second)
                {
                    this->m_velocity.x *= -1;
                }
                if (closestX == rect->m_pos.x ||
                    closestX == rect->m_pos.x + rect->getDimensions().first)
                {
                    this->m_velocity.y *= -1;
                }
                is_collision = true;
                m_logger.info("COLLISION WITH A RECTANGLE");
            }
        }
        else
        {
            game::object::CircleObject* circle = dynamic_cast<game::object::CircleObject*>(obj);
            auto distanceSquared =
                (circle->m_pos.x - this->m_pos.x) * (circle->m_pos.x - this->m_pos.x) +
                (circle->m_pos.y - this->m_pos.y) * (circle->m_pos.y - this->m_pos.y);
            if (distanceSquared <=
                (circle->m_radius + this->m_radius) * (circle->m_radius + this->m_radius))
            {
                is_collision = true;
                m_logger.info("COLLISION WITH A CIRCLE\n");

                this->m_velocity.x *= -1;
                this->m_velocity.y *= -1;
            }
        }
    }
    return is_collision;
}
}  // namespace game::object