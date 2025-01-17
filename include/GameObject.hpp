#ifndef GAME_OBJECT_H
#define GAME_OBJECT_H

#include "ClassLogger.hpp"
#include "ObjectHelper.hpp"

#include <algorithm>
#include <cmath>
#include <memory>
#include <vector>
#define BORDER_COLLISION
namespace game::object
{
using Force = helper::Vector2D;
using Position = helper::Vector2D;
using Velocity = helper::Vector2D;
using Acceleration = helper::Vector2D;
using Color = helper::Color_t;
using ColorState = helper::ObjectColor;
using ObjectType = helper::ObjectType;

constexpr double MIN_VELOCITY = -500;
constexpr double MAX_VELOCITY = 500;

// forward declaration
class CircleObject;
class RectObject;

#define LAST_POSITION_SIZE 100
class GameObject
{

  public:
    explicit GameObject(std::string& logger_name) : m_logger(logger_name)
    {
        m_color_state = helper::ObjectColor::RED;
        m_color = {255, 0, 0, 255};
        m_mass = 1.0f;
        m_force = {0.0f, 0.0f};
        m_velocity = {0.1f, 0.1f};
        m_pos = {0.0f, 0.0f};
        m_acceleration = {0.0f, 0.0f};
        m_restitution = 1;
    };
    virtual ~GameObject() {}

    virtual void update(float delta_time, int screen_width,
                        int screen_height) = 0;  // Pure virtual function
    void update_position(float delta_time);
    void update_velocity(float delta_time);
    void update_force();
    void update_acceleration();
    void update_physics(float delta_time);
    virtual bool border_collision(int screen_width, int screen_height) = 0;
    bool is_colliding_with(const GameObject& other) const;
    void on_collision(GameObject& other);

    void destroy();
    void update_color(float delta_time);  // DONE
    void setPosition(Position pos);
    void setVelocity(Velocity velocity);
    void setAcceleration(Acceleration acceleration);
    void addAcceleration(const Acceleration& acceleration);
    void setForce(Force force);
    void addForce(const Force& force);
    void setMass(float mass);
    void setRestitution(double restitution);
    void setColor(Color color);
    void setColorState(ColorState colorState);
    std::vector<Position> getLastPositions() const;
    Force getForce() const;
    double get_mass();

    ObjectType get_type() const;
    Color get_color() const;
    Position getPosition() const;
    Velocity getVelocity() const;
    Acceleration getAcceleration() const;
    virtual Position getCenter() const = 0;

  protected:
    ClassLogger m_logger;
    Position m_pos;
    Velocity m_velocity;
    Acceleration m_acceleration;
    ObjectType m_type;
    double m_mass;
    double m_restitution;  //  Coefficient of restitution (bounciness) for elastic/inelastic
                           //  collisions. 1.0 for perfectly elastic, 0.0 for perfectly inelastic.
    Force m_force;
    ColorState m_color_state;
    Color m_color;

  private:
    std::vector<Position> m_last_positions;
};
}  // namespace game::object
#endif