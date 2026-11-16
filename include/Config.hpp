#pragma once
#include "PhysicsWorld.hpp"

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
    int randomObjects = 0;  // > 0: start with N random objects instead of the default scene.
    int maxObjects = 200;   // Upper bound for random spawning.

    // Sandbox UI
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

void printUsage(std::ostream& out, const char* program);
void printConfig(std::ostream& out, const Config& config);
}  // namespace game
