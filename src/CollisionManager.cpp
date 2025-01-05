#include <CollisionManager.hpp>
namespace game::engine
{
void CollisionManager::resolve_collisions(
    const std::vector<std::unique_ptr<game::object::GameObject>>& objects, float delta_time)
{
    // Temporary set to track current collisions
    std::vector<std::pair<game::object::GameObject*, game::object::GameObject*>> currentCollisions;

    for (size_t i = 0; i < objects.size(); ++i)
    {
        for (size_t j = i + 1; j < objects.size(); ++j)
        {
            if (objects[i]->is_colliding_with(*objects[j]))
            {
                std::cout << "before collision velocity x: " << objects[i]->getVelocity().x
                          << " y: " << objects[i]->getVelocity().y << std::endl;
                std::cout << "before collision velocity x: " << objects[j]->getVelocity().x
                          << " y: " << objects[j]->getVelocity().y << std::endl;
                objects[i]->on_collision(*objects[j], delta_time);
                objects[j]->on_collision(*objects[i], delta_time);
                std::cout << "Collision detected between objects " << i << " and " << j
                          << std::endl;
                std::cout << "after collision velocity x: " << objects[i]->getVelocity().x
                          << " y: " << objects[i]->getVelocity().y << std::endl;
                std::cout << "after collision velocity x: " << objects[j]->getVelocity().x
                          << " y: " << objects[j]->getVelocity().y << std::endl;
                // Track the colliding pair
                currentCollisions.emplace_back(objects[i].get(), objects[j].get());
            }
            else
            {
                activeCollisions.erase(
                    std::remove_if(activeCollisions.begin(), activeCollisions.end(),
                                   [&](const std::pair<game::object::GameObject*,
                                                       game::object::GameObject*>& pair)
                                   {
                                       return (pair.first == objects[i].get() &&
                                               pair.second == objects[j].get()) ||
                                              (pair.first == objects[j].get() &&
                                               pair.second == objects[i].get());
                                   }),
                    activeCollisions.end());
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