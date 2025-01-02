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

  private:
    void setDimensions(int width, int height);
    int m_width, m_height;
};
}  // namespace game::object
#endif