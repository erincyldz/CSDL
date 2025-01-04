#include <CircleObject.hpp>
#include <GameObject.hpp>
#include <RectObject.hpp>
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
            if (this->m_color.b <= 0)
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

bool GameObject::is_colliding_with(const GameObject& other) const
{
    if (this->m_type == ObjectType::CIRCLE && other.m_type == ObjectType::CIRCLE)
    {
        // Circle-Circle collision
        float dx = this->m_pos.x - other.m_pos.x;
        float dy = this->m_pos.y - other.m_pos.y;
        float distance = std::sqrt(dx * dx + dy * dy);
        float combinedRadii = static_cast<const CircleObject*>(this)->m_radius +
                              static_cast<const CircleObject*>(&other)->m_radius;
        return distance < combinedRadii;
    }
    else if (this->m_type == ObjectType::RECTANGLE && other.m_type == ObjectType::RECTANGLE)
    {
        // Rectangle-Rectangle collision (AABB)
        const RectObject* rect1 = static_cast<const RectObject*>(this);
        const RectObject* rect2 = static_cast<const RectObject*>(&other);
        int rect_1_width = rect1->getDimensions().first;
        int rect_1_height = rect1->getDimensions().second;
        int rect_2_width = rect2->getDimensions().first;
        int rect_2_height = rect2->getDimensions().second;

        return !(rect1->m_pos.x + rect_1_width < rect2->m_pos.x ||
                 rect1->m_pos.x > rect2->m_pos.x + rect_2_width ||
                 rect1->m_pos.y + rect_1_height < rect2->m_pos.y ||
                 rect1->m_pos.y > rect2->m_pos.y + rect_2_height);
    }
    else if (this->m_type == ObjectType::CIRCLE && other.m_type == ObjectType::RECTANGLE)
    {
        // Circle-Rectangle collision
        const CircleObject* circle = static_cast<const CircleObject*>(this);
        const RectObject* rect = static_cast<const RectObject*>(&other);
        int rect_width = rect->getDimensions().first;
        int rect_height = rect->getDimensions().second;
        float nearestX =
            std::max(rect->m_pos.x, std::min(circle->m_pos.x, rect->m_pos.x + rect_width));
        float nearestY =
            std::max(rect->m_pos.y, std::min(circle->m_pos.y, rect->m_pos.y + rect_height));

        float dx = circle->m_pos.x - nearestX;
        float dy = circle->m_pos.y - nearestY;

        return (dx * dx + dy * dy) < (circle->m_radius * circle->m_radius);
    }
    else if (this->m_type == ObjectType::RECTANGLE && other.m_type == ObjectType::CIRCLE)
    {
        // Rectangle-Circle collision (reusing the logic by swapping)
        return other.is_colliding_with(*this);
    }

    return false;  // Default: no collision
}

void GameObject::on_collision(GameObject& other)
{
    // Placeholder for collision handling
    m_logger.info("Collision detected between objects");
    // Simplified elastic collision response
    float normalX = other.m_pos.x - this->m_pos.x;
    float normalY = other.m_pos.y - this->m_pos.y;
    float magnitude = std::sqrt(normalX * normalX + normalY * normalY);
    normalX /= magnitude;
    normalY /= magnitude;

    float relativeVelocityX = other.m_velocity.x - this->m_velocity.x;
    float relativeVelocityY = other.m_velocity.y - this->m_velocity.y;

    float velocityAlongNormal = relativeVelocityX * normalX + relativeVelocityY * normalY;

    if (velocityAlongNormal > 0)
        return;  // Objects are separating

    float e = std::min(this->m_restitution, other.m_restitution);

    float impulse = (-(1 + e) * velocityAlongNormal) / (1 / this->m_mass + 1 / other.m_mass);

    float impulseX = impulse * normalX;
    float impulseY = impulse * normalY;

    this->m_velocity.x -= impulseX / this->m_mass;
    this->m_velocity.y -= impulseY / this->m_mass;

    other.m_velocity.x += impulseX / other.m_mass;
    other.m_velocity.y += impulseY / other.m_mass;
}

}  // namespace game::object