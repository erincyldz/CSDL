#pragma once
#include "Audio.hpp"
#include "ClassLogger.hpp"
#include "Config.hpp"
#include "FrameClock.hpp"
#include "ImGuiLayer.hpp"
#include "Input.hpp"
#include "Menu.hpp"
#include "PhysicsWorld.hpp"
#include "Sandbox.hpp"
#include "SandboxPanel.hpp"
#include "Scenario.hpp"
#include "SdlRaii.hpp"
#include "SdlRenderer.hpp"
#include "Window.hpp"

#include <optional>
#include <random>
#include <string>

namespace game
{
enum class GameState
{
    Menu,
    Settings,
    Playing,
    Paused
};

// Owns all subsystems and runs the main loop. Member order matters: SDL is initialized first
// and shut down last; the ImGui layer is destroyed before the renderer it draws with.
class Game
{
  public:
    explicit Game(Config config);
    void run();

  private:
    enum MenuId : int
    {
        MENU_PLAY,
        MENU_SETTINGS,
        MENU_EXIT,
        SET_GRAVITY,
        SET_BORDER,
        SET_TRAILS,
        SET_DIRECTION,
        SET_HIGHLIGHTS,
        SET_COLOR_CYCLE,
        SET_SOUND,
        SET_BACK
    };

    // What R (reset) reloads.
    struct SceneSource
    {
        std::optional<std::string> file;
        scenario::Preset preset = scenario::Preset::Default;
        int randomCount = 0;
    };

    bool inGame() const
    {
        return m_state == GameState::Playing || m_state == GameState::Paused;
    }

    void processInput();
    void handleCommand(Command command);
    void handleMenuClick(int x, int y);
    void toggleSetting(int id);
    void refreshSettingsLabels();
    void drawUi();
    void applyPanelActions(const ui::PanelActions& actions);
    void applyConfig();
    void updatePhysics();
    void render();

    void resetScene();
    void onSceneLoaded();
    void spawnRandomObject();
    void kickObjects();

    // Width of the area left of the docked sandbox panel; this is the physics world's width.
    int simulationWidth() const;
    RenderOptions renderOptions() const;
    SandboxOverlay sandboxOverlay() const;

    Config m_config;
    ClassLogger m_logger{"Game"};
    std::string m_assetDir;

    sdl::SdlContext m_sdl;
    sdl::Window m_window;
    sdl::Audio m_audio;
    sdl::SdlRenderer m_renderer;
    ui::ImGuiLayer m_imgui;
    Input m_input;
    FrameClock m_clock;

    physics::PhysicsWorld m_world;
    sandbox::Sandbox m_sandbox;
    ui::SandboxPanel m_panel;
    ui::Menu m_mainMenu{"CSDL"};
    ui::Menu m_settingsMenu{"Settings"};
    SceneSource m_sceneSource;

    GameState m_state = GameState::Menu;
    bool m_running = true;
    bool m_stepRequested = false;
    double m_fps = 0.0;
    double m_frameTime = 0.0;
    std::mt19937 m_rng;
};
}  // namespace game
