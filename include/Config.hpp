#pragma once
#include "PhysicsWorld.hpp"
#include "Scenario.hpp"

#include <cstdint>
#include <iosfwd>
#include <string>
#include <string_view>

namespace game
{
// Runtime configuration. Every field can be set from a config file (`key = value`) or from the
// command line (`--key=value`, `--key value`, `--flag`, `--no-flag`). See printUsage().
struct Config
{
    // Window / timing
    int windowWidth = 1280;
    int windowHeight = 800;
    int physicsHz = 120;  // Fixed physics steps per second (independent from render FPS).
    bool vsync = true;
    int maxFps = 0;          // Extra frame cap when > 0 (useful with vsync off).
    double timeScale = 1.0;  // Simulation speed multiplier (slow motion < 1 < fast forward).

    // Scene
    std::string scene;      // Scene file to load at start (and on reset).
    std::string preset;     // Built-in preset key (see scenario::presetKey); empty: default.
    int randomObjects = 0;  // > 0: start with N random objects instead of the default scene.
    int maxObjects = 200;   // Upper bound for random spawning.
    int seed = 0;           // Seed for random scenes; 0 picks a new one on every start.

    // Headless mode: physics only (no window, sound or rendering), prints statistics and exits.
    bool headless = false;
    double duration = 10.0;       // Simulated seconds.
    double reportInterval = 1.0;  // Simulated seconds between statistics lines (0: end only).
    std::string saveScene;        // Write the final world to this scene file.

    // Sandbox UI
    bool skipMenu = false;  // Start the simulation directly instead of the main menu.
    bool showUi = true;
    double uiScale = 1.25;

    // Physics
    bool gravity = true;
    double gravitationalConstant = 100.0;
    double softening = 5.0;
    bool borderCollision = true;
    double maxSpeed = 500.0;
    double restitutionThreshold = 20.0;

    // Rendering
    bool renderTrails = true;
    bool renderDirection = true;
    bool renderCollisionHighlights = true;
    bool colorCycle = false;

    // Audio
    bool sound = true;
    int volume = 10;        // 0..100
    std::string musicPath;  // Optional, relative to the asset directory or absolute.

    // Misc
    std::string assetDir;  // Empty: auto-detect next to the executable.
    std::string logLevel = "info";
};

enum class ParseResult
{
    Ok,
    ExitSuccess,  // e.g. --help was printed
    ExitFailure
};

ParseResult parseArgs(Config& config, int argc, char* argv[], std::ostream& err);
bool loadConfigFile(Config& config, const std::string& path, std::string& error);
bool setConfigValue(Config& config, std::string_view key, std::string_view value,
                    std::string& error);
bool validateConfig(const Config& config, std::string& error);

physics::PhysicsSettings physicsSettingsFrom(const Config& config);
// The scene to start with: `scene` if set, else `preset`, else random objects or the default.
// Assumes a validated config.
scenario::SceneSource startSceneFrom(const Config& config);
// The configured seed, or a fresh one from std::random_device when it is 0.
std::uint32_t resolveSeed(int seed);

void printUsage(std::ostream& out, const char* program);
void printConfig(std::ostream& out, const Config& config);
}  // namespace game
