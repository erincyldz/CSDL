#include <CircleObject.hpp>
#include <GameObject.hpp>
#include <RectObject.hpp>
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
    if(!m_is_stable)
    {
        m_velocity = velocity;
    }
    else
    {
        m_velocity *= 0;
    }
}
void GameObject::setPosition(Position pos)
{
    m_pos = pos;
}
void GameObject::setRestitution(double rest)
{
    m_restitution = rest;
}
void GameObject::setMass(float mass)
{
    m_mass = mass;
}
void GameObject::setForce(Force force)
{
    m_force = force;
}

void GameObject::addForce(const Force& force)
{
    m_force += force;
}

void GameObject::setColor(Color color)
{
    m_color = color;
}
void GameObject::setColorState(ColorState colorState)
{
    m_color_state = colorState;
}
void GameObject::setStability(bool stable)
{
    if(stable)
    {
        m_velocity *= 0;
    }
    m_is_stable = stable;
}
bool GameObject::getStability() const
{
    return m_is_stable;
}
double GameObject::getMass() const
{
    return m_mass;
}
ObjectType GameObject::getType() const
{
    return m_type;
}
Color GameObject::getColor() const
{
    return m_color;
}
Position GameObject::getPosition() const
{
    return m_pos;
}
Velocity GameObject::getVelocity() const
{
    return m_velocity;
}

std::vector<Position> GameObject::getLastPositions() const
{
    return m_last_positions;
}

bool GameObject::is_colliding_with(const GameObject& other) const
{
    if (this->m_type == ObjectType::CIRCLE && other.m_type == ObjectType::CIRCLE)
    {
        // Circle-Circle collision
        auto diff_vector = m_pos - other.m_pos;
        float distance = diff_vector.magnitude();
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

        return !(rect1->m_pos.getX() + rect_1_width < rect2->m_pos.getX() ||
                 rect1->m_pos.getX() > rect2->m_pos.getX() + rect_2_width ||
                 rect1->m_pos.getY() + rect_1_height < rect2->m_pos.getY() ||
                 rect1->m_pos.getY() > rect2->m_pos.getY() + rect_2_height);
    }
    else if (this->m_type == ObjectType::CIRCLE && other.m_type == ObjectType::RECTANGLE)
    {
        // Circle-Rectangle collision
        const CircleObject* circle = static_cast<const CircleObject*>(this);
        const RectObject* rect = static_cast<const RectObject*>(&other);
        int rect_width = rect->getDimensions().first;
        int rect_height = rect->getDimensions().second;
        float nearestX = std::max(rect->m_pos.getX(),
                                  std::min(circle->m_pos.getX(), rect->m_pos.getX() + rect_width));
        float nearestY = std::max(rect->m_pos.getY(),
                                  std::min(circle->m_pos.getY(), rect->m_pos.getY() + rect_height));

        float dx = circle->m_pos.getX() - nearestX;
        float dy = circle->m_pos.getY() - nearestY;

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
    // TODO: this method `MUST` apply the law of force with the conservation of the Momentum.
    //  but current situation doesnt seem applt the law of force.
    //  this method should be reconsidered with the physicicst observation.
    //  what about a p_1 + p_2 = p_1' + p_2'
    //  or m1_v1 + m2_v2 = m1_v1' + m2_v2'
    //  you need to calculate the collision angle, you have dot product.
    //  im not sure but implementation of restituation might be wrong.

    // Calculate normal vector between objects

    auto diff_vector = other.m_pos - this->m_pos;

    double magnitude = diff_vector.magnitude();
    // Prevent division by zero or exact overlap
    if (magnitude == 0.0f)
    {
        magnitude = 0.01f;
        diff_vector.setX(1.0);
        diff_vector.setY(0.0);
    }

    diff_vector = diff_vector.normalized();

    // Relative velocity
    auto relative_velocity = other.m_velocity - this->m_velocity;

    // Project the relative velocity onto the collision normal
    double velocityAlongNormal = relative_velocity.dot(diff_vector);

    // Skip resolving if objects are separating
    if (velocityAlongNormal > 0)
        return;
    double e = std::min(this->m_restitution, other.m_restitution);
    // Calculate impulse scalar
    double impulse = (-(1 + e) * velocityAlongNormal) / (1 / this->m_mass + 1 / other.m_mass);

    // Apply impulse along the normal
    auto impulse_vector = diff_vector * impulse;
    auto speed_this = impulse_vector / this->m_mass;
    auto speed_other = impulse_vector / other.m_mass;
    const double velocityQuantizationStep = 0.01;
    if(!getStability())
    {
        this->m_velocity -= game::object::helper::Vector2D(
            std::round(speed_this.getX() / velocityQuantizationStep) * velocityQuantizationStep,
            std::round(speed_this.getY() / velocityQuantizationStep) * velocityQuantizationStep);
    }
    // Quantize the velocity of the other object
    if(!other.getStability())
    {
        other.m_velocity += game::object::helper::Vector2D(
            std::round(speed_other.getX() / velocityQuantizationStep) * velocityQuantizationStep,
             std::round(speed_other.getY() / velocityQuantizationStep) * velocityQuantizationStep);
    }
}

Acceleration GameObject::getAcceleration() const
{
    return m_acceleration;
}

Force GameObject::getForce() const
{
    return m_force;
}

void GameObject::setAcceleration(Acceleration acceleration)
{
    m_acceleration = acceleration;
}

void GameObject::addAcceleration(const Acceleration& acceleration)
{
    m_acceleration += acceleration;
}
void GameObject::update_force()
{
    m_force *= (1 - FRICTION_COEFFICIENT);
}

void GameObject::update_acceleration()
{
    if (m_mass != 0)
    {
        m_acceleration = m_force / m_mass;
        m_force = {0, 0};  // reset the m_force after implementation
    }
    else
    {
        m_logger.error("GameObject has zero mass. Acceleration update skipped.");
    }
}

void GameObject::update_velocity(float delta_time)
{
    m_velocity += m_acceleration * delta_time;
    m_logger.debug("\nCurrent speed x: {}\nCurrent speed y: {}\nCurrent magnitude:{}", m_velocity.getX(), m_velocity.getY(),m_velocity.magnitude());
}

void GameObject::update_position(float delta_time)
{
    auto x_speed = std::clamp(m_velocity.getX(), MIN_VELOCITY, MAX_VELOCITY);
    auto y_speed = std::clamp(m_velocity.getY(), MIN_VELOCITY, MAX_VELOCITY);
    if(!getStability())
    {
        m_velocity.setX(x_speed);
        m_velocity.setY(y_speed);
    }
    else
    {
        m_velocity *= 0;
    }
    m_pos += m_velocity * delta_time;
}

void GameObject::update_physics(float delta_time)
{
    // 1. Update force corresponding to friction factor.
    update_force();

    // 2. Update the acceleration based on the current force and mass
    update_acceleration();

    // 3. Update the velocity using the newly calculated acceleration
    update_velocity(delta_time);

    // 4. Update the position based on the updated velocity
    update_position(delta_time);

    // keep the last 10 positions
    if (m_last_positions.size() == LAST_POSITION_SIZE)
    {
        m_last_positions.erase(m_last_positions.begin());
    }
    m_last_positions.push_back(this->getCenter());

    // m_logger.info("Position: {} ", this->m_pos);
}
}  // namespace game::object