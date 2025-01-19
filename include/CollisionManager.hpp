#ifndef COLLISION_MANAGER_H
#define COLLISION_MANAGER_H
#include "GameObject.hpp"

#include <algorithm>
#include <cmath>
#include <memory>
#include <vector>
namespace game::engine
{
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

class CollisionManager
{

    using SpatialGrid =
        std::unordered_map<std::pair<int, int>, std::vector<game::object::GameObject*>, pair_hash>;
    using CollisionsObject =
        std::vector<std::pair<game::object::GameObject*, game::object::GameObject*>>;

  private:
    std::vector<std::pair<game::object::GameObject*, game::object::GameObject*>>
        m_active_collisions;
    int m_cumulative_collision_count = 0;
    const std::vector<std::pair<int, int>> neighborOffsets = {
        {-1, -1}, {-1, 0}, {-1, 1}, {0, -1}, {0, 0}, {0, 1}, {1, -1}, {1, 0}, {1, 1}};

  public:
    CollisionManager() = default;
    ~CollisionManager() = default;
    void resolve_collisions(const std::vector<std::unique_ptr<game::object::GameObject>>& objects);
    void calculate_gravitational_force(
        const std::vector<std::unique_ptr<game::object::GameObject>>& objects);
    const std::vector<std::pair<game::object::GameObject*, game::object::GameObject*>>&
        get_active_collisions() const;
    int get_collision_count() const;
};
}  // namespace game::engine

#endif