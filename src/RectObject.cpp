#include <RectObject.hpp>

namespace game::object
{
RectObject::RectObject(int width, int height, std::string& logger_name)
    : GameObject(logger_name), m_width(width), m_height(height)
{
    m_type = helper::ObjectType::RECTANGLE;
};

std::pair<int, int> RectObject::getDimensions() const
{
    return std::pair<int, int>(m_width, m_height);
}

void RectObject::setDimensions(int width, int height)
{
    m_width = width;
    m_height = height;
}

void RectObject::tellRadius()
{
    m_logger.error("Width is : {} \t Height is : {}", this->m_width, this->m_height);
}
}  // namespace game::object