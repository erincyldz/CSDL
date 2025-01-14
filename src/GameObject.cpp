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
    m_velocity = velocity;
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
    m_force.x += force.x;
    m_force.y += force.y;
}

void GameObject::setColor(Color color)
{
    m_color = color;
}
void GameObject::setColorState(ColorState colorState)
{
    m_color_state = colorState;
}
double GameObject::get_mass()
{
    return m_mass;
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
    // Calculate normal vector between objects

    double normalX = other.m_pos.x - this->m_pos.x;
    double normalY = other.m_pos.y - this->m_pos.y;

    double magnitude = std::sqrt(normalX * normalX + normalY * normalY);
    // Prevent division by zero or exact overlap
    if (magnitude == 0.0f)
    {
        magnitude = 0.01f;
        normalX = 1.0f;
        normalY = 0.0f;
    }

    normalX /= magnitude;
    normalY /= magnitude;

    // Relative velocity
    double relativeVelocityX = other.m_velocity.x - this->m_velocity.x;
    double relativeVelocityY = other.m_velocity.y - this->m_velocity.y;

    // Project the relative velocity onto the collision normal
    double velocityAlongNormal = relativeVelocityX * normalX + relativeVelocityY * normalY;

    // Skip resolving if objects are separating
    if (velocityAlongNormal > 0)
        return;
    double e = std::min(this->m_restitution, other.m_restitution);
    // Calculate impulse scalar
    double impulse = (-(1 + e) * velocityAlongNormal) / (1 / this->m_mass + 1 / other.m_mass);

    // Apply impulse along the normal
    double impulseX = impulse * normalX;
    double impulseY = impulse * normalY;
    auto xSpeedThis = impulseX / this->m_mass;
    auto ySpeedThis = impulseY / this->m_mass;
    auto xSpeedOther = impulseX / other.m_mass;
    auto ySpeedOther = impulseY / other.m_mass;
    const double velocityQuantizationStep = 0.01;

    this->m_velocity.x -=
        std::round(xSpeedThis / velocityQuantizationStep) * velocityQuantizationStep;
    this->m_velocity.y -=
        std::round(ySpeedThis / velocityQuantizationStep) * velocityQuantizationStep;

    other.m_velocity.x +=
        std::round(xSpeedOther / velocityQuantizationStep) * velocityQuantizationStep;
    other.m_velocity.y +=
        std::round(ySpeedOther / velocityQuantizationStep) * velocityQuantizationStep;
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
    m_acceleration.x += acceleration.x;
    m_acceleration.y += acceleration.y;
}

void GameObject::update_acceleration()
{
    if (m_mass != 0)
    {
        m_acceleration.x = m_force.x / m_mass;
        m_acceleration.y = m_force.y / m_mass;
        m_force = {0, 0};  // reset the m_force after implementation
    }
    else
    {
        // This is a non-sense case
        // so dont need to be handled
    }
}

void GameObject::update_velocity(float delta_time)
{
    m_velocity.x += m_acceleration.x * delta_time;
    m_velocity.y += m_acceleration.y * delta_time;
    m_velocity.x *= (1.0F - FRICTION_COEFFICIENT);
    m_velocity.y *= (1.0F - FRICTION_COEFFICIENT);
    // if (m_velocity.x <= 1e-6)
    // {
    //     m_velocity.x = 0;
    // }
    // if (m_velocity.y <= 1e-6)
    // {
    //     m_velocity.y = 0;
    // }
}

void GameObject::update_position(float delta_time)
{
    std::clamp(m_velocity.x, MIN_VELOCITY, MAX_VELOCITY);
    std::clamp(m_velocity.y, MIN_VELOCITY, MAX_VELOCITY);
    m_pos.x += m_velocity.x * delta_time;
    m_pos.y += m_velocity.y * delta_time;
}

void GameObject::update_physics(float delta_time)
{

    // 1. Update the acceleration based on the current force and mass
    update_acceleration();

    // 2. Update the velocity using the newly calculated acceleration
    update_velocity(delta_time);

    // 3. Update the position based on the updated velocity
    update_position(delta_time);

    std::cout << "Position: " << m_pos.x << " " << m_pos.y << std::endl;

    // keep the last 10 positions
    if (m_last_positions.size() == LAST_POSITION_SIZE)
    {
        m_last_positions.erase(m_last_positions.begin());
    }
    m_last_positions.push_back(m_pos);
}
void GameObject::apply_gravitational_force(float delta_time)
{
    Position new_pos;
    new_pos.x = {this->getPosition().x + (this->m_force.x * delta_time / this->m_mass)};
    new_pos.y = {this->getPosition().y + (this->m_force.y * delta_time / this->m_mass)};
    this->setPosition(new_pos);
}
}  // namespace game::object