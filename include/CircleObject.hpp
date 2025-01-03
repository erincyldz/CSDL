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
    void update(float delta_time, int screen_width,
                int screen_height) override;  // Use `override` for safety
    virtual bool collision_detection(int screen_width, int screen_height) override;

  private:
    void setRadius(float rad);
    float m_radius;
};
}  // namespace game::object
#endif