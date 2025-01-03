#ifndef RECT_OBJECT_H
#define RECT_OBJECT_H
#include <GameObject.hpp>
namespace game::object
{
class RectObject : public GameObject
{
  public:
    RectObject(int width, int height, std::string& logger_name);
    std::pair<int, int> getDimensions() const;
    void tellRadius();
    void update(float delta_time, int screen_width, int screen_height) override;
    virtual bool collision_detection(int screen_width, int screen_height) override;

  private:
    void setDimensions(int width, int height);
    int m_width, m_height;
};
}  // namespace game::object
#endif