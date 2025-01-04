#include <CollisionManager.hpp>
namespace game::engine
{
void CollisionManager::resolve_collisions(
    const std::vector<std::unique_ptr<game::object::GameObject>>& objects)
{
    for (size_t i = 0; i < objects.size(); ++i)
    {
        for (size_t j = i + 1; j < objects.size(); ++j)
        {
            if (objects[i]->is_colliding_with(*objects[j]))
            {
                objects[i]->on_collision(*objects[j]);
                objects[j]->on_collision(*objects[i]);

                // Add the colliding pair to the list
                activeCollisions.emplace_back(objects[i].get(), objects[j].get());
            }
        }
    }
}

const std::vector<std::pair<game::object::GameObject*, game::object::GameObject*>>&
    CollisionManager::get_active_collisions() const
{
    return activeCollisions;
}
}  // namespace game::engine