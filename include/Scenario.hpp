#pragma once
#include "GameObject.hpp"
#include "PhysicsWorld.hpp"

#include <random>

namespace game::scenario
{
// Built-in test scenes. The world's bounds and physics settings must be set before loading.
enum class Preset
{
    Default,        // Original two-body setup
    Empty,          // Nothing; build your own with the Spawn tool
    Random,         // N random circles and rectangles
    CircularOrbit,  // Planets on circular orbits around a stable sun (tests gravity + integrator)
    HeadOn,         // Head-on collisions with equal / unequal masses (tests impulses)
    NewtonsCradle,  // A row of touching balls hit from one side (tests momentum transfer)
    Container       // Stable walls forming a box with a pile of objects inside
};
constexpr int PRESET_COUNT = 7;
const char* presetName(Preset preset);
const char* presetDescription(Preset preset);
void loadPreset(physics::PhysicsWorld& world, Preset preset, std::mt19937& rng, int randomCount);

// A small body orbiting / bouncing off a heavy stable one (the original main.cpp setup).
void loadDefault(physics::PhysicsWorld& world);

// `count` random circles and rectangles spread over the world.
void loadRandom(physics::PhysicsWorld& world, std::mt19937& rng, int count);

object::GameObject makeRandomObject(std::mt19937& rng, double worldWidth, double worldHeight);
}  // namespace game::scenario
