#ifndef GAME_OBJECT_H
#define GAME_OBJECT_H
#include "ClassLogger.hpp"
namespace game::object
{
enum class ObjectType
{
    RECTANGLE = 0,
    CIRCLE
};
class GameObject
{
  public:
    explicit GameObject(std::string& logger_name) : m_logger(logger_name) {};

  private:
    int m_posX, m_posY;

  protected:
    ObjectType m_type;
    ClassLogger m_logger;
};
}  // namespace game::object
#endif