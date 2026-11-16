#include "Menu.hpp"

#include <algorithm>

namespace game::ui
{
void Menu::addItem(int id, std::string label, object::Color color)
{
    m_items.push_back(MenuItem{id, std::move(label), color, Rect{}});
}

void Menu::setLabel(int id, std::string label)
{
    for (auto& item : m_items)
        if (item.id == id)
            item.label = std::move(label);
}

void Menu::layout(int windowWidth, int windowHeight)
{
    const int count = static_cast<int>(m_items.size());
    const int itemWidth = std::min(MAX_ITEM_WIDTH, windowWidth - 40);
    const int listHeight = count * ITEM_HEIGHT + std::max(0, count - 1) * ITEM_GAP;
    const int totalHeight = TITLE_SPACE + listHeight;
    const int top = std::max(10, (windowHeight - totalHeight) / 2);

    m_titleY = top;
    int y = top + TITLE_SPACE;
    for (auto& item : m_items)
    {
        item.rect = Rect{(windowWidth - itemWidth) / 2, y, itemWidth, ITEM_HEIGHT};
        y += ITEM_HEIGHT + ITEM_GAP;
    }
}

std::optional<int> Menu::hitTest(int x, int y) const
{
    for (const auto& item : m_items)
        if (item.rect.contains(x, y))
            return item.id;
    return std::nullopt;
}
}  // namespace game::ui
