#ifndef CIRCLE_OBJECT_H
#define CIRCLE_OBJECT_H
#include <GameObject.hpp>
#include <RectObject.hpp>

namespace game::object
{
class CircleObject : public GameObject
{
  public:
    CircleObject(float rad, std::string& logger_name);
    CircleObject(std::string& logger_name);
    float getRadius() const;
    void setRadius(float rad);

    void tellRadius();
    void update(float delta_time, int screen_width,
                int screen_height) override;  // Use `override` for safety
    bool border_collision(int screen_width, int screen_height) override;
    float m_radius;

  private:
};
}  // namespace game::object
#endif