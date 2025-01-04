#ifndef COLLISION_MANAGER_H
#define COLLISION_MANAGER_H
#include "GameObject.hpp"

#include <algorithm>
#include <cmath>
#include <memory>
#include <vector>
namespace game::engine
{
class CollisionManager
{
  private:
    std::vector<std::pair<game::object::GameObject*, game::object::GameObject*>> activeCollisions;

  public:
    CollisionManager() = default;
    ~CollisionManager() = default;
    void resolve_collisions(const std::vector<std::unique_ptr<game::object::GameObject>>& objects);
    const std::vector<std::pair<game::object::GameObject*, game::object::GameObject*>>&
        get_active_collisions() const;
};
}  // namespace game::engine

#endif