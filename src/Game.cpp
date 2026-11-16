#include "Game.hpp"

#include "SceneIO.hpp"

#include <filesystem>
#include <iostream>

namespace game
{
namespace fs = std::filesystem;

namespace
{
constexpr object::Color PLAY_COLOR{0, 160, 0, 255};
constexpr object::Color SETTINGS_COLOR{0, 0, 200, 255};
constexpr object::Color EXIT_COLOR{200, 0, 0, 255};
constexpr object::Color TOGGLE_COLOR{60, 60, 90, 255};
constexpr object::Color BACK_COLOR{110, 110, 110, 255};

// Finds the asset directory: explicit config value, next to the executable (CMake copies it
// there), the parent of the executable directory (bin/ inside the source tree) or the CWD.
std::string resolveAssetDir(const Config& config)
{
    if (!config.assetDir.empty())
        return config.assetDir;

    std::vector<fs::path> candidates;
    if (char* base = SDL_GetBasePath())
    {
        const fs::path basePath(base);
        SDL_free(base);
        candidates.push_back(basePath / "assets");
        candidates.push_back(basePath / ".." / "assets");
    }
    candidates.emplace_back("assets");

    for (const auto& c : candidates)
    {
        std::error_code ec;
        if (fs::is_directory(c, ec))
            return fs::weakly_canonical(c, ec).string();
    }
    return "assets";
}

std::string assetPath(const std::string& assetDir, const std::string& relative)
{
    if (relative.empty())
        return {};
    const fs::path p(relative);
    return p.is_absolute() ? relative : (fs::path(assetDir) / p).string();
}

const char* onOff(bool value)
{
    return value ? "ON" : "OFF";
}
}  // namespace

Game::Game(Config config)
    : m_config(std::move(config)), m_assetDir(resolveAssetDir(m_config)),
      m_window("CSDL", m_config.windowWidth, m_config.windowHeight, m_config.vsync),
      m_audio(m_config.sound, m_config.volume, assetPath(m_assetDir, "sounds/8bitCollision.wav"),
              assetPath(m_assetDir, m_config.musicPath)),
      m_renderer(m_window, assetPath(m_assetDir, "fonts/SmartDuck.ttf")),
      m_imgui(m_window.handle(), m_window.renderer(), static_cast<float>(m_config.uiScale)),
      m_clock(1.0 / m_config.physicsHz), m_world(physicsSettingsFrom(m_config)),
      m_panel(static_cast<float>(m_config.uiScale)), m_rng(std::random_device{}())
{
    m_logger.info("Asset directory: {}", m_assetDir);
    m_clock.setTimeScale(m_config.timeScale);

    m_mainMenu.addItem(MENU_PLAY, "Play", PLAY_COLOR);
    m_mainMenu.addItem(MENU_SETTINGS, "Settings", SETTINGS_COLOR);
    m_mainMenu.addItem(MENU_EXIT, "Exit", EXIT_COLOR);

    for (int id = SET_GRAVITY; id <= SET_SOUND; ++id)
        m_settingsMenu.addItem(id, "", TOGGLE_COLOR);
    m_settingsMenu.addItem(SET_BACK, "Back", BACK_COLOR);
    refreshSettingsLabels();

    m_world.setBounds(simulationWidth(), m_window.height());

    if (!m_config.scene.empty())
        m_sceneSource.file = m_config.scene;
    else if (m_config.randomObjects > 0)
    {
        m_sceneSource.preset = scenario::Preset::Random;
        m_sceneSource.randomCount = std::min(m_config.randomObjects, m_config.maxObjects);
    }
    resetScene();
    if (m_config.skipMenu)
        m_state = GameState::Playing;
}

void Game::run()
{
    printConfig(std::cout, m_config);
    m_logger.info("Game is running");

    while (m_running)
    {
        m_frameTime = m_clock.tick();
        if (m_frameTime > 0.0)
            m_fps = (m_fps == 0.0) ? 1.0 / m_frameTime : 0.9 * m_fps + 0.1 / m_frameTime;

        processInput();
        m_imgui.beginFrame();
        drawUi();
        m_world.setBounds(simulationWidth(), m_window.height());
        if (inGame())
        {
            m_sandbox.validate(m_world);
            m_sandbox.update(m_world, m_frameTime);
        }
        updatePhysics();
        render();

        m_clock.limitFrameRate(m_config.maxFps);
    }
    m_logger.info("Shutting down");
}

void Game::processInput()
{
    const InputFrame input = m_input.poll(
        [this](const SDL_Event& e)
        {
            return m_imgui.processEvent(e);
        });

    if (input.resized)
        m_window.onResize(input.resized->width, input.resized->height);
    for (const Command c : input.commands)
        handleCommand(c);

    if (input.mouseMoved)
        m_sandbox.onMouseMove(input.mouseMoved->x, input.mouseMoved->y);
    for (const MouseButtonEvent& b : input.buttons)
    {
        if (inGame())
            m_sandbox.onMouseButton(m_world, b, m_rng);
        else if (b.pressed && b.button == MouseButton::Left)
            handleMenuClick(b.x, b.y);
    }
}

void Game::handleCommand(Command command)
{
    switch (command)
    {
        case Command::Quit:
            m_running = false;
            break;
        case Command::Back:
            // From the main menu ESC quits; everywhere else it returns to the menu (the scene
            // is kept, "Play" resumes it).
            if (m_state == GameState::Menu)
                m_running = false;
            else
                m_state = GameState::Menu;
            break;
        case Command::TogglePause:
            if (m_state == GameState::Playing)
                m_state = GameState::Paused;
            else if (m_state == GameState::Paused)
                m_state = GameState::Playing;
            break;
        case Command::Reset:
            if (inGame())
                resetScene();
            break;
        case Command::SpawnRandom:
            if (inGame())
                spawnRandomObject();
            break;
        case Command::Kick:
            if (inGame())
                kickObjects();
            break;
        case Command::DeleteSelected:
            if (inGame())
                m_sandbox.deleteSelected(m_world);
            break;
        case Command::StepOnce:
            if (m_state == GameState::Paused)
                m_stepRequested = true;
            break;
        case Command::ToggleUi:
            m_config.showUi = !m_config.showUi;
            break;
    }
}

void Game::handleMenuClick(int x, int y)
{
    if (m_state == GameState::Menu)
    {
        m_mainMenu.layout(m_window.width(), m_window.height());
        switch (m_mainMenu.hitTest(x, y).value_or(-1))
        {
            case MENU_PLAY:
                m_state = GameState::Playing;
                break;
            case MENU_SETTINGS:
                m_state = GameState::Settings;
                break;
            case MENU_EXIT:
                m_running = false;
                break;
            default:
                break;
        }
    }
    else if (m_state == GameState::Settings)
    {
        m_settingsMenu.layout(m_window.width(), m_window.height());
        if (const auto id = m_settingsMenu.hitTest(x, y))
        {
            if (*id == SET_BACK)
                m_state = GameState::Menu;
            else
                toggleSetting(*id);
        }
    }
}

void Game::toggleSetting(int id)
{
    switch (id)
    {
        case SET_GRAVITY:
            m_config.gravity = !m_config.gravity;
            break;
        case SET_BORDER:
            m_config.borderCollision = !m_config.borderCollision;
            break;
        case SET_TRAILS:
            m_config.renderTrails = !m_config.renderTrails;
            break;
        case SET_DIRECTION:
            m_config.renderDirection = !m_config.renderDirection;
            break;
        case SET_HIGHLIGHTS:
            m_config.renderCollisionHighlights = !m_config.renderCollisionHighlights;
            break;
        case SET_COLOR_CYCLE:
            m_config.colorCycle = !m_config.colorCycle;
            break;
        case SET_SOUND:
            m_config.sound = !m_config.sound;
            break;
        default:
            return;
    }
    applyConfig();
}

void Game::refreshSettingsLabels()
{
    auto label = [](const char* name, bool value)
    {
        return std::string(name) + ": " + onOff(value);
    };
    m_settingsMenu.setLabel(SET_GRAVITY, label("Gravity", m_config.gravity));
    m_settingsMenu.setLabel(SET_BORDER, std::string("Borders: ") +
                                            (m_config.borderCollision ? "BOUNCE" : "WRAP"));
    m_settingsMenu.setLabel(SET_TRAILS, label("Trails", m_config.renderTrails));
    m_settingsMenu.setLabel(SET_DIRECTION, label("Velocity arrows", m_config.renderDirection));
    m_settingsMenu.setLabel(SET_HIGHLIGHTS,
                            label("Collision outlines", m_config.renderCollisionHighlights));
    m_settingsMenu.setLabel(SET_COLOR_CYCLE, label("Color cycle", m_config.colorCycle));
    m_settingsMenu.setLabel(SET_SOUND, label("Sound", m_config.sound));
}

// Pushes the (possibly edited) Config into every subsystem.
void Game::applyConfig()
{
    m_world.setSettings(physicsSettingsFrom(m_config));
    m_audio.setEnabled(m_config.sound);
    m_audio.setVolume(m_config.volume);
    m_clock.setFixedStep(1.0 / m_config.physicsHz);
    m_clock.setTimeScale(m_config.timeScale);
    refreshSettingsLabels();
}

void Game::drawUi()
{
    if (!inGame() || !m_config.showUi)
        return;
    ui::PanelContext ctx{m_world, m_config, m_sandbox, m_state == GameState::Paused, m_fps};
    applyPanelActions(m_panel.draw(ctx));
}

void Game::applyPanelActions(const ui::PanelActions& a)
{
    if (a.physicsChanged || a.audioChanged || a.timingChanged)
        applyConfig();
    if (a.togglePause)
        handleCommand(Command::TogglePause);
    if (a.step)
        m_stepRequested = true;
    if (a.deleteSelected)
        m_sandbox.deleteSelected(m_world);
    if (a.duplicateSelected)
        m_sandbox.duplicateSelected(m_world);
    if (a.clear)
    {
        m_world.clear();
        onSceneLoaded();
        m_panel.setStatus("World cleared (R reloads the previous scene)", false);
    }
    if (a.reset)
        resetScene();
    if (a.loadPreset)
    {
        m_sceneSource = SceneSource{std::nullopt, *a.loadPreset, m_panel.randomCount()};
        resetScene();
    }
    if (a.saveScene)
    {
        std::string error;
        if (scene::save(m_world, *a.saveScene, error))
            m_panel.setStatus(
                "Saved " + std::to_string(m_world.size()) + " objects to " + *a.saveScene, false);
        else
            m_panel.setStatus("Save failed: " + error, true);
    }
    if (a.loadScene)
    {
        std::string error;
        if (scene::load(m_world, *a.loadScene, error))
        {
            m_sceneSource.file = *a.loadScene;
            onSceneLoaded();
            m_panel.setStatus("Loaded " + std::to_string(m_world.size()) + " objects from " +
                                  *a.loadScene,
                              false);
        }
        else
            m_panel.setStatus("Load failed: " + error, true);
    }
}

void Game::updatePhysics()
{
    std::size_t newContacts = 0;
    bool stepped = false;
    while (m_clock.consumeStep())
    {
        if (m_state != GameState::Playing)
            continue;  // Drain the accumulator so we don't fast-forward after a pause.
        m_world.step(m_clock.fixedStep());
        newContacts += m_world.newContactsLastStep();
        stepped = true;
    }
    if (m_stepRequested && m_state == GameState::Paused)
    {
        m_world.step(m_clock.fixedStep());
        newContacts += m_world.newContactsLastStep();
        stepped = true;
    }
    m_stepRequested = false;

    if (newContacts > 0)
        m_audio.playCollision();
    if (stepped)
        m_panel.recordEnergy(m_world.computeEnergy().total());
}

int Game::simulationWidth() const
{
    int width = m_window.width();
    if (m_config.showUi)
        width -= static_cast<int>(m_panel.width());
    return std::max(width, 100);
}

RenderOptions Game::renderOptions() const
{
    RenderOptions options;
    options.trails = m_config.renderTrails;
    options.direction = m_config.renderDirection;
    options.collisionHighlights = m_config.renderCollisionHighlights;
    return options;
}

SandboxOverlay Game::sandboxOverlay() const
{
    SandboxOverlay overlay;
    overlay.selected = m_sandbox.selected();
    const bool mouseOverUi = m_config.showUi && m_imgui.wantsMouse();
    if (!mouseOverUi && !m_sandbox.isGrabbing())
        overlay.hovered = m_sandbox.hovered(m_world);

    if (m_sandbox.tool == sandbox::Tool::Spawn)
    {
        if (const auto line = m_sandbox.spawnDragLine())
        {
            overlay.dragLine = std::make_pair(line->from, line->to);
            overlay.ghostShape = m_sandbox.spawn.shape();
            overlay.ghostPosition = line->from;
        }
        else if (!mouseOverUi)
        {
            overlay.ghostShape = m_sandbox.spawn.shape();
            overlay.ghostPosition = m_sandbox.mouse();
        }
    }
    return overlay;
}

void Game::render()
{
    m_renderer.beginFrame();
    switch (m_state)
    {
        case GameState::Menu:
            m_mainMenu.layout(m_window.width(), m_window.height());
            m_renderer.drawMenu(m_mainMenu);
            break;
        case GameState::Settings:
            m_settingsMenu.layout(m_window.width(), m_window.height());
            m_renderer.drawMenu(m_settingsMenu);
            break;
        case GameState::Playing:
        case GameState::Paused:
        {
            m_renderer.drawWorld(m_world, renderOptions());
            m_renderer.drawSandboxOverlay(m_world, sandboxOverlay());
            HudInfo hud;
            hud.collisions = m_world.totalCollisions();
            hud.objects = m_world.size();
            hud.fps = m_fps;
            hud.viewWidth = simulationWidth();
            m_renderer.drawHud(hud);
            if (m_state == GameState::Paused)
                m_renderer.drawPausedOverlay(hud.viewWidth);
            break;
        }
    }
    m_imgui.render(m_window.renderer());
    m_renderer.endFrame();
}

void Game::resetScene()
{
    if (m_sceneSource.file)
    {
        std::string error;
        if (scene::load(m_world, *m_sceneSource.file, error))
        {
            onSceneLoaded();
            m_logger.info("Scene loaded from {} ({} objects)", *m_sceneSource.file, m_world.size());
            return;
        }
        m_logger.error("Failed to load scene: {}", error);
        m_panel.setStatus("Load failed: " + error, true);
        m_sceneSource = SceneSource{};  // Fall back to the default preset.
    }
    scenario::loadPreset(m_world, m_sceneSource.preset, m_rng, m_sceneSource.randomCount);
    m_panel.setPreset(m_sceneSource.preset);
    onSceneLoaded();
    m_panel.setStatus(std::string("Loaded preset: ") + scenario::presetName(m_sceneSource.preset),
                      false);
    m_logger.info("Scene reset ({} objects)", m_world.size());
}

void Game::onSceneLoaded()
{
    m_sandbox.select(std::nullopt);
    m_sandbox.validate(m_world);
    m_panel.clearEnergyHistory();
}

void Game::spawnRandomObject()
{
    if (static_cast<int>(m_world.size()) >= m_config.maxObjects)
    {
        m_logger.warning("Object limit reached ({}); raise --max-objects to add more.",
                         m_config.maxObjects);
        return;
    }
    m_world.add(scenario::makeRandomObject(m_rng, m_world.width(), m_world.height()));
}

void Game::kickObjects()
{
    std::uniform_real_distribution<double> angle(0.0, 2.0 * math::PI);
    constexpr double KICK_SPEED = 150.0;
    for (auto& obj : m_world.objects())
    {
        const double a = angle(m_rng);
        obj.setVelocity(obj.getVelocity() + math::Vector2D{std::cos(a), std::sin(a)} * KICK_SPEED);
    }
}
}  // namespace game
