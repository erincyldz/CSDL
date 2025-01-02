#ifndef CIRCLE_OBJECT_H
#define CIRCLE_OBJECT_H
#include <GameObject.hpp>
namespace game::object
{
class CircleObject : public GameObject
{
  public:
    CircleObject(float rad, std::string& logger_name);
    float getRadius();
    void tellRadius();
    void update(float delta_time) override;  // Use `override` for safety

  private:
    void setRadius(float rad);
    float m_radius;
};
}  // namespace game::object
#endif