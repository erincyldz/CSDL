#ifndef IRENDERER_HPP
#define IRENDERER_HPP

#include <GameObject.hpp>
#include <SDL2/SDL.h>
#include <memory>
#include <vector>

namespace game
{
class IRenderer
{
  public:
    virtual ~IRenderer() = default;

    // // Render all game objects
    // virtual void renderGameObjects(
    //     const std::vector<std::unique_ptr<game::object::GameObject>>& gameObjects) = 0;

    // Highlight specific game objects for collision
    virtual void renderCollisionHighlights(
        const std::vector<std::pair<game::object::GameObject*, game::object::GameObject*>>&
            collisions) = 0;
};
}  // namespace game

#endif  // IRENDERER_HPP