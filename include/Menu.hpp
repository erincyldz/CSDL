#pragma once
#include "GameObject.hpp"

#include <optional>
#include <string>
#include <utility>
#include <vector>

namespace game::ui
{
struct Rect
{
    int x = 0;
    int y = 0;
    int w = 0;
    int h = 0;

    bool contains(int px, int py) const
    {
        return px >= x && px < x + w && py >= y && py < y + h;
    }
};

struct MenuItem
{
    int id;
    std::string label;
    object::Color color;
    Rect rect;
};

// A vertical list of buttons centered in the window. The same layout is used for drawing and
// hit-testing, so they can never get out of sync.
class Menu
{
  public:
    explicit Menu(std::string title) : m_title(std::move(title)) {}

    void addItem(int id, std::string label, object::Color color);
    void setLabel(int id, std::string label);
    void layout(int windowWidth, int windowHeight);
    std::optional<int> hitTest(int x, int y) const;

    const std::string& title() const
    {
        return m_title;
    }
    int titleY() const
    {
        return m_titleY;
    }
    const std::vector<MenuItem>& items() const
    {
        return m_items;
    }

  private:
    static constexpr int ITEM_HEIGHT = 44;
    static constexpr int ITEM_GAP = 12;
    static constexpr int MAX_ITEM_WIDTH = 320;
    static constexpr int TITLE_SPACE = 64;

    std::string m_title;
    std::vector<MenuItem> m_items;
    int m_titleY = 0;
};
}  // namespace game::ui
