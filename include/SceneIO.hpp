#pragma once
#include "PhysicsWorld.hpp"

#include <string>

// Plain-text scene files, one object per line:
//   circle name=Ball x=300 y=200 vx=10 vy=0 radius=15 mass=700 restitution=1 stable=0 color=255,0,0
//   rect   name=Wall x=500 y=300 vx=0 vy=0 width=20 height=300 mass=1000 restitution=0.5 stable=1
// '#' starts a comment. Missing keys keep their defaults.
namespace game::scene
{
bool save(const physics::PhysicsWorld& world, const std::string& path, std::string& error);
// On success the world's objects are replaced. On error the world is left untouched.
bool load(physics::PhysicsWorld& world, const std::string& path, std::string& error);
}  // namespace game::scene
