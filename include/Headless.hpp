#pragma once
#include "Config.hpp"

#include <iosfwd>

// Headless mode: runs the physics without SDL (no window, sound, rendering or input) for a fixed
// amount of simulated time and prints statistics. With the same seed, scene and build, two runs
// print the same numbers, apart from the wall time line.
namespace game::headless
{
enum ExitCode : int
{
    EXIT_OK = 0,
    EXIT_SETUP_ERROR = 1,  // The scene could not be loaded or saved.
    EXIT_DIVERGED = 2      // An object's position or velocity became NaN or infinite.
};

// Statistics go to `out`, errors to `err`. Returns one of ExitCode.
int run(const Config& config, std::ostream& out, std::ostream& err);
}  // namespace game::headless
