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
    struct pair_hash
    {
        template<class T1, class T2>
        std::size_t operator()(const std::pair<T1, T2>& pair) const
        {
            auto h1 = std::hash<T1>{}(pair.first);
            auto h2 = std::hash<T2>{}(pair.second);
            return h1 ^ (h2 << 1);  // Combine hashes
        }
    };
};
}  // namespace game::engine

#endif