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

void RectObject::update(float delta_time, int screen_width, int screen_height,
                        const std::vector<game::object::GameObject*>& other_objects)
{
    update_color(delta_time);
    m_pos.x += m_velocity.x * delta_time;
    m_pos.y += m_velocity.y * delta_time;
    border_collision(screen_width, screen_height);
    collision_detection(std::move(other_objects));
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
    for (auto& obj : other_objects)
    {
        if (obj->m_type == game::object::ObjectType::RECTANGLE)
        {
            game::object::RectObject* rect = dynamic_cast<game::object::RectObject*>(obj);
            if (!(this->m_pos.x + this->m_width < rect->m_pos.x ||
                  rect->m_pos.x + rect->m_width < this->m_pos.x ||
                  this->m_pos.y + this->m_height < rect->m_pos.y ||
                  rect->m_pos.y + rect->m_height < this->m_pos.y))
            {
                this->m_width /= 2;
                this->m_height /= 2;
                m_logger.info("COLLISION WITH A RECTANGLE\n");
            }
        }
        else
        {
            game::object::CircleObject* circle = dynamic_cast<game::object::CircleObject*>(obj);
            auto closestX =
                std::max(this->m_pos.x, std::min(circle->m_pos.x, this->m_pos.x + this->m_width));
            auto closestY =
                std::max(this->m_pos.y, std::min(circle->m_pos.y, this->m_pos.y + this->m_height));
            auto distanceSquared = (circle->m_pos.x - closestX) * (circle->m_pos.x - closestX) +
                                   (circle->m_pos.y - closestY) * (circle->m_pos.y - closestY);
            if (distanceSquared <= circle->m_radius * circle->m_radius)
            {
                m_logger.info("COLLISION WITH A CIRCLE\n");

                this->m_velocity.x *= -1;
                this->m_velocity.y *= -1;
            }
        }
    }
}
}  // namespace game::object