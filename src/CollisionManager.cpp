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
                    pos_x = static_cast<int>(std::floor(circle->getPosition().getX() / cellSize));
                    pos_y = static_cast<int>(std::floor(circle->getPosition().getY() / cellSize));
                }
                break;
            case game::object::helper::ObjectType::RECTANGLE:
                if (const auto* rect = dynamic_cast<const game::object::RectObject*>(&obj))
                {
                    pos_x = static_cast<int>(std::floor(
                        (rect->getPosition().getX() + rect->get_width() / 2) / cellSize));
                    pos_y = static_cast<int>(std::floor(
                        (rect->getPosition().getY() + rect->get_height() / 2) / cellSize));
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

static double d_square(double num)
{
    return num * num;
}

void CollisionManager::calculate_gravitational_force(
    const std::vector<std::unique_ptr<game::object::GameObject>>& objects)
{
    auto t = get_active_collisions();
    for (auto& obj1 : objects)
    {
        game::object::Force applied_force{0, 0};

        for (auto& obj2 : objects)
        {
            for (auto& collision : t)
            {
                for (auto coll : {collision.first, collision.second})
                {
                    if (coll == obj1.get() || coll == obj2.get())
                    {
                        return;
                    }
                }
            }
            if (obj1 == obj2)
                continue;  // Skip self-collision
            double x_axis_force, y_axis_force = 0;
            double distance =
                sqrt(d_square(obj2->getPosition().getX() - obj1->getPosition().getX()) +
                     d_square(obj2->getPosition().getY() - obj1->getPosition().getY()));
            double g_force =
                (GRAVITATIONAL_CONSTANT * obj1->get_mass() * obj2->get_mass()) / d_square(distance);
            game::object::helper::Vector2D force{0, 0};

            if (!distance)
            {
                force.setX(0);
                force.setY(0);
            }
            else
            {
                force = (obj2->getPosition() - obj1->getPosition()) / distance;
            }
            applied_force += force * g_force;
        }
        obj1->addForce(applied_force);
    }
}
}  // namespace game::engine