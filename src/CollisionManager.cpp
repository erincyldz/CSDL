#include <CircleObject.hpp>
#include <CollisionManager.hpp>
#include <RectObject.hpp>
namespace game::engine
{
void CollisionManager::resolve_collisions(
    const std::vector<std::unique_ptr<game::object::GameObject>>& objects)
{
    const float cellSize = 50.0f;  // Adjust based on object size and game world

    SpatialGrid spatialGrid;  // A map to store objects by their grid cell

    // Helper lambda to compute grid cell for a position
    auto getCell = [cellSize](const game::object::GameObject& obj)
    {
        game::object::Position center = obj.getCenter();
        int cellX = static_cast<int>(std::floor(center.getX() / cellSize));
        int cellY = static_cast<int>(std::floor(center.getY() / cellSize));
        return std::make_pair(cellX, cellY);
    };

    // Populate the spatial grid
    for (const auto& obj : objects)
    {
        auto cell = getCell(*obj);
        spatialGrid[cell].push_back(obj.get());
    }

    // Temporary set to track current collisions
    CollisionsObject currentCollisions;

    // Check collisions within each cell and neighboring cells
    for (const auto& [cell, cellObjects] : spatialGrid)
    {
        for (const auto& offset : this->neighborOffsets)
        {
            auto neighborCell =
                std::make_pair(cell.first + offset.first, cell.second + offset.second);
            if (spatialGrid.find(neighborCell) == spatialGrid.end())
            {
                continue;
            }
            const auto& neighborObjects = spatialGrid[neighborCell];

            // Check for collisions between objects in the current cell and neighbor cell
            for (auto* obj1 : cellObjects)
            {
                for (auto* obj2 : neighborObjects)
                {
                    if (obj1 >= obj2)
                    {
                        continue;  // Skip self-collision and duplicate checks
                    }

                    if (obj1->is_colliding_with(*obj2))
                    {
                        currentCollisions.emplace_back(obj1, obj2);
                        m_cumulative_collision_count++;
                    }
                    else
                    {
                        m_active_collisions.erase(
                            std::remove_if(m_active_collisions.begin(), m_active_collisions.end(),
                                           [&](const std::pair<game::object::GameObject*,
                                                               game::object::GameObject*>& pair)
                                           {
                                               return (pair.first == obj1 && pair.second == obj2) ||
                                                      (pair.first == obj2 && pair.second == obj1);
                                           }),
                            m_active_collisions.end());
                    }
                }
            }
        }
    }

    // Resolve Collisions
    for (const auto& [obj1, obj2] : currentCollisions)
    {
        obj1->on_collision(*obj2);
        obj2->on_collision(*obj1);
    }

    // Update active collisions
    m_active_collisions = std::move(currentCollisions);
}
const std::vector<std::pair<game::object::GameObject*, game::object::GameObject*>>&
    CollisionManager::get_active_collisions() const
{
    return m_active_collisions;
}

int CollisionManager::get_collision_count() const
{
    return m_cumulative_collision_count;
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