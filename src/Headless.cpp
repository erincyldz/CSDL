#include "Headless.hpp"

#include "SceneIO.hpp"

#include <algorithm>
#include <chrono>
#include <cmath>
#include <cstdint>
#include <cstdio>
#include <optional>
#include <ostream>
#include <random>
#include <string>

namespace game::headless
{
namespace
{
using physics::PhysicsWorld;

bool isFinite(const math::Vector2D& v)
{
    return std::isfinite(v.getX()) && std::isfinite(v.getY());
}

// The first object whose state is no longer a number, if any.
const object::GameObject* findNonFinite(const PhysicsWorld& world)
{
    for (const auto& obj : world.objects())
        if (!isFinite(obj.getPosition()) || !isFinite(obj.getVelocity()))
            return &obj;
    return nullptr;
}

// Energy change relative to the start, or nullopt when the start energy is ~0 (no scale).
std::optional<double> relativeDrift(double start, double now)
{
    if (!std::isfinite(start) || !std::isfinite(now) || std::abs(start) < 1e-12)
        return std::nullopt;
    return (now - start) / std::abs(start);
}

std::string formatDrift(std::optional<double> drift)
{
    if (!drift)
        return "n/a";
    char buf[32];
    std::snprintf(buf, sizeof(buf), "%+.4f%%", *drift * 100.0);
    return buf;
}

void printTableHeader(std::ostream& out)
{
    char buf[160];
    std::snprintf(buf, sizeof(buf), "%10s %8s %11s %9s %13s %13s %13s %11s\n", "time [s]",
                  "objects", "collisions", "contacts", "kinetic", "potential", "total", "drift");
    out << buf;
}

void printRow(std::ostream& out, const PhysicsWorld& world, double time, double startEnergy)
{
    const auto energy = world.computeEnergy();
    char buf[160];
    std::snprintf(buf, sizeof(buf), "%10.3f %8zu %11llu %9zu %13.6g %13.6g %13.6g %11s\n", time,
                  world.size(), static_cast<unsigned long long>(world.totalCollisions()),
                  world.activeContacts().size(), energy.kinetic, energy.potential, energy.total(),
                  formatDrift(relativeDrift(startEnergy, energy.total())).c_str());
    out << buf;
}
}  // namespace

int run(const Config& config, std::ostream& out, std::ostream& err)
{
    PhysicsWorld world(physicsSettingsFrom(config));
    world.setBounds(config.windowWidth, config.windowHeight);

    const std::uint32_t seed = resolveSeed(config.seed);
    std::mt19937 rng(seed);

    // Unlike the windowed app, a scene file that fails to load is an error here: a script that
    // asked for a scene must not silently get the default one.
    const scenario::SceneSource source = startSceneFrom(config);
    std::string sceneLabel;
    if (source.file)
    {
        std::string error;
        if (!scene::load(world, *source.file, error))
        {
            err << "Failed to load scene: " << error << '\n';
            return EXIT_SETUP_ERROR;
        }
        sceneLabel = "file " + *source.file;
    }
    else
    {
        scenario::loadPreset(world, source.preset, rng, source.randomCount);
        sceneLabel = std::string("preset ") + scenario::presetKey(source.preset);
    }

    const double dt = 1.0 / config.physicsHz;
    const std::int64_t totalSteps =
        std::max<std::int64_t>(1, std::llround(config.duration * config.physicsHz));
    const std::int64_t reportEvery =
        config.reportInterval > 0.0
            ? std::max<std::int64_t>(1, std::llround(config.reportInterval * config.physicsHz))
            : 0;

    const auto& settings = world.settings();
    out << "CSDL headless run\n"
        << "  scene:   " << sceneLabel << " (" << world.size() << " objects)\n"
        << "  seed:    " << seed << '\n'
        << "  world:   " << world.width() << " x " << world.height() << " px, gravity "
        << (settings.gravity ? "on" : "off") << ", borders "
        << (settings.borderCollision ? "bounce" : "wrap") << '\n'
        << "  physics: " << config.physicsHz << " Hz, " << totalSteps << " steps ("
        << static_cast<double>(totalSteps) * dt << " s simulated)\n\n";

    const auto startEnergy = world.computeEnergy();
    printTableHeader(out);
    printRow(out, world, 0.0, startEnergy.total());

    const auto wallStart = std::chrono::steady_clock::now();
    for (std::int64_t step = 1; step <= totalSteps; ++step)
    {
        world.step(dt);
        const double time = static_cast<double>(step) * dt;

        if (const auto* bad = findNonFinite(world))
        {
            printRow(out, world, time, startEnergy.total());
            err << "Simulation diverged at t=" << time << " s (step " << step << "): object '"
                << bad->getName() << "' (id " << bad->getId()
                << ") has a non-finite position or velocity\n";
            return EXIT_DIVERGED;
        }
        if (step == totalSteps || (reportEvery > 0 && step % reportEvery == 0))
            printRow(out, world, time, startEnergy.total());
    }
    const double wallSeconds =
        std::chrono::duration<double>(std::chrono::steady_clock::now() - wallStart).count();

    const auto endEnergy = world.computeEnergy();
    const double simulated = static_cast<double>(totalSteps) * dt;
    char buf[200];
    out << "\nSummary\n";
    std::snprintf(buf, sizeof(buf), "  simulated:  %.3f s (%lld steps)\n", simulated,
                  static_cast<long long>(totalSteps));
    out << buf;
    // The only line that changes between identical runs; filter it out to compare outputs.
    std::snprintf(buf, sizeof(buf), "  wall time:  %.3f s (%.0f steps/s, %.1fx real time)\n",
                  wallSeconds, static_cast<double>(totalSteps) / std::max(wallSeconds, 1e-9),
                  simulated / std::max(wallSeconds, 1e-9));
    out << buf;
    std::snprintf(buf, sizeof(buf), "  objects:    %zu\n", world.size());
    out << buf;
    std::snprintf(buf, sizeof(buf), "  collisions: %llu\n",
                  static_cast<unsigned long long>(world.totalCollisions()));
    out << buf;
    std::snprintf(buf, sizeof(buf), "  energy:     %.6g -> %.6g (drift %s)\n", startEnergy.total(),
                  endEnergy.total(),
                  formatDrift(relativeDrift(startEnergy.total(), endEnergy.total())).c_str());
    out << buf;
    std::snprintf(buf, sizeof(buf), "  momentum:   |p| %.6g -> %.6g\n",
                  startEnergy.momentum.magnitude(), endEnergy.momentum.magnitude());
    out << buf;

    // Positions and velocities can stay finite while the energy overflows (v^2 > DBL_MAX).
    if (!std::isfinite(startEnergy.total()) || !std::isfinite(endEnergy.total()))
    {
        err << "Simulation diverged: the total energy is not finite\n";
        return EXIT_DIVERGED;
    }

    if (!config.saveScene.empty())
    {
        std::string error;
        if (!scene::save(world, config.saveScene, error))
        {
            err << "Failed to save scene: " << error << '\n';
            return EXIT_SETUP_ERROR;
        }
        out << "  saved:      " << config.saveScene << '\n';
    }
    return EXIT_OK;
}
}  // namespace game::headless
