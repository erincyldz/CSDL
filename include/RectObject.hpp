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
    std::pair<int, int> getDimensions() const;
    void tellRadius();
    void update(float delta_time, int screen_width, int screen_height,
                const std::vector<game::object::GameObject*>& other_objects) override;
    bool border_collision(int screen_width, int screen_height) override;
    bool collides_with(const std::vector<game::object::GameObject*>& other_objects) override;

  private:
    void setDimensions(int width, int height);
    int m_width, m_height;
    float m_rotation_rad;  // the angle of rotation in rad
};
}  // namespace game::object
#endif