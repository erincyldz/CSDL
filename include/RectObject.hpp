#ifndef RECT_OBJECT_H
#define RECT_OBJECT_H
#include <CircleObject.hpp>
#include <GameObject.hpp>
namespace game::object
{
class RectObject : public GameObject
{
  public:
    RectObject(int width, int height, std::string& logger_name);
    RectObject(std::string& logger_name);
    std::pair<int, int> getDimensions() const;
    void tellRadius();
    void update(float delta_time, int screen_width, int screen_height) override;
    bool border_collision(int screen_width, int screen_height) override;
    int get_width() const;
    int get_height() const;
    void setDimensions(int width, int height);

  private:
    int m_width, m_height;
    float m_rotation_rad;  // the angle of rotation in rad
};
}  // namespace game::object
#endif