#include <CircleObject.hpp>
#include <CollisionManager.hpp>
#include <RectObject.hpp>
namespace game::engine
{
void CollisionManager::resolve_collisions(
    const std::vector<std::unique_ptr<game::object::GameObject>>& objects)
{
    // Define the grid size
    const float cellSize = 50.0f;  // Adjust based on object size and game world

    // A map to store objects by their grid cell
    std::unordered_map<std::pair<int, int>, std::vector<game::object::GameObject*>, pair_hash>
        spatialGrid;

    // Helper lambda to compute grid cell for a position
    auto getCell = [cellSize](const game::object::GameObject& obj)
    {
        int pos_x, pos_y;
        switch (obj.get_type())
        {
            case game::object::helper::ObjectType::CIRCLE:
                if (const auto* circle = dynamic_cast<const game::object::CircleObject*>(&obj))
                {
                    pos_x = static_cast<int>(std::floor(circle->getPosition().x / cellSize));
                    pos_y = static_cast<int>(std::floor(circle->getPosition().y / cellSize));
                }
                break;
            case game::object::helper::ObjectType::RECTANGLE:
                if (const auto* rect = dynamic_cast<const game::object::RectObject*>(&obj))
                {
                    pos_x = static_cast<int>(
                        std::floor((rect->getPosition().x + rect->get_width() / 2) / cellSize));
                    pos_y = static_cast<int>(
                        std::floor((rect->getPosition().y + rect->get_height() / 2) / cellSize));
                }
                break;
        }

        int cellX = static_cast<int>(std::floor(pos_x / cellSize));
        int cellY = static_cast<int>(std::floor(pos_y / cellSize));
        return std::make_pair(cellX, cellY);
    };

    // Populate the spatial grid
    for (const auto& obj : objects)
    {
        auto cell = getCell(*obj);
        spatialGrid[cell].push_back(obj.get());
    }

    // Temporary set to track current collisions
    std::vector<std::pair<game::object::GameObject*, game::object::GameObject*>> currentCollisions;

    // Check collisions within each cell and neighboring cells
    for (const auto& [cell, cellObjects] : spatialGrid)
    {
        const int cellX = cell.first;
        const int cellY = cell.second;

        // Iterate over the cell and its 8 neighbors
        for (int dx = -1; dx <= 1; ++dx)
        {
            for (int dy = -1; dy <= 1; ++dy)
            {
                auto neighborCell = std::make_pair(cellX + dx, cellY + dy);
                if (spatialGrid.find(neighborCell) == spatialGrid.end())
                    continue;

                const auto& neighborObjects = spatialGrid[neighborCell];

                // Check for collisions between objects in the current cell and neighbor cell
                for (auto* obj1 : cellObjects)
                {
                    for (auto* obj2 : neighborObjects)
                    {
                        if (obj1 == obj2)
                            continue;  // Skip self-collision
                        if (obj1->is_colliding_with(*obj2))
                        {
                            obj1->on_collision(*obj2);
                            obj2->on_collision(*obj1);
                            // Track the colliding pair
                            currentCollisions.emplace_back(obj1, obj2);
                        }
                        else
                        {
                            activeCollisions.erase(
                                std::remove_if(
                                    activeCollisions.begin(), activeCollisions.end(),
                                    [&](const std::pair<game::object::GameObject*,
                                                        game::object::GameObject*>& pair)
                                    {
                                        return (pair.first == obj1 && pair.second == obj2) ||
                                               (pair.first == obj2 && pair.second == obj1);
                                    }),
                                activeCollisions.end());
                        }
                    }
                }
            }
        }
    }

    // Update active collisions
    activeCollisions = std::move(currentCollisions);
}
const std::vector<std::pair<game::object::GameObject*, game::object::GameObject*>>&
    CollisionManager::get_active_collisions() const
{
    return activeCollisions;
}
}  // namespace game::engine