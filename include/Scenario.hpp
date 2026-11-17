#pragma once
#include "GameObject.hpp"
#include "PhysicsWorld.hpp"

#include <optional>
#include <random>
#include <string>
#include <string_view>

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
constexpr int DEFAULT_RANDOM_COUNT = 20;  // Object count of the Random preset unless one is given.
const char* presetName(Preset preset);
const char* presetDescription(Preset preset);
// Short lower-case name used in config files and on the command line (e.g. "head_on").
const char* presetKey(Preset preset);
// Accepts the key, with '-' or '_' as the separator. Returns nullopt for an unknown name.
std::optional<Preset> presetFromKey(std::string_view key);
// "default, empty, random, ..." for help texts and error messages.
std::string presetKeyList();
void loadPreset(physics::PhysicsWorld& world, Preset preset, std::mt19937& rng, int randomCount);

// What a reset reloads: a scene file if set, otherwise a preset.
struct SceneSource
{
    std::optional<std::string> file;
    Preset preset = Preset::Default;
    int randomCount = 0;
};

// A small body orbiting / bouncing off a heavy stable one (the original main.cpp setup).
void loadDefault(physics::PhysicsWorld& world);

// `count` random circles and rectangles spread over the world.
void loadRandom(physics::PhysicsWorld& world, std::mt19937& rng, int count);

object::GameObject makeRandomObject(std::mt19937& rng, double worldWidth, double worldHeight);
}  // namespace game::scenario
