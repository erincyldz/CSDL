#pragma once
#include "Config.hpp"
#include "PhysicsWorld.hpp"
#include "Sandbox.hpp"
#include "Scenario.hpp"

#include <array>
#include <optional>
#include <string>

namespace game::ui
{
// What the user asked for this frame; Game applies these after drawing the panel.
struct PanelActions
{
    bool togglePause = false;
    bool step = false;
    bool reset = false;
    bool clear = false;
    bool deleteSelected = false;
    bool duplicateSelected = false;
    bool physicsChanged = false;  // Physics or render settings in Config were edited
    bool audioChanged = false;
    bool timingChanged = false;  // physics_hz or time_scale
    std::optional<scenario::Preset> loadPreset;
    std::optional<std::string> saveScene;
    std::optional<std::string> loadScene;
};

struct PanelContext
{
    physics::PhysicsWorld& world;
    Config& config;
    sandbox::Sandbox& sandbox;
    bool paused;
    double fps;
};

// The Dear ImGui sandbox window. Edits objects, the spawn template and Config directly; scene
// level operations are returned as PanelActions.
class SandboxPanel
{
  public:
    explicit SandboxPanel(float scale) : m_scale(scale), m_width(defaultWidth(scale)) {}

    PanelActions draw(PanelContext& ctx);

    static float defaultWidth(float scale)
    {
        return 340.0f * scale;
    }
    // Current width of the panel docked on the right edge of the window.
    float width() const
    {
        return m_width;
    }
    void setPreset(scenario::Preset preset)
    {
        m_presetIndex = static_cast<int>(preset);
    }

    void recordEnergy(double totalEnergy);
    void clearEnergyHistory();
    void setStatus(std::string message, bool isError);
    int randomCount() const
    {
        return m_randomCount;
    }

  private:
    void drawSimulationControls(PanelContext& ctx, PanelActions& actions);
    void drawTools(PanelContext& ctx);
    void drawSpawnTemplate(PanelContext& ctx);
    void drawInspector(PanelContext& ctx, PanelActions& actions);
    void drawObjectList(PanelContext& ctx);
    void drawScene(PanelContext& ctx, PanelActions& actions);
    void drawPhysics(PanelContext& ctx, PanelActions& actions);
    void drawRendering(PanelContext& ctx, PanelActions& actions);
    void drawStats(PanelContext& ctx);

    float m_scale;
    float m_width;
    int m_presetIndex = 0;
    int m_randomCount = 20;
    char m_scenePath[256] = "scene.txt";
    std::string m_status;
    bool m_statusIsError = false;

    static constexpr int HISTORY = 600;
    std::array<float, HISTORY> m_energy{};
    int m_energyOffset = 0;
    int m_energyCount = 0;
};
}  // namespace game::ui
