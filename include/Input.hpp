#pragma once
#include <functional>
#include <optional>
#include <vector>

union SDL_Event;

namespace game
{
enum class Command
{
    Quit,            // Window closed
    Back,            // ESC
    TogglePause,     // P
    Reset,           // R: restart the current scene
    SpawnRandom,     // Q: add a random object
    Kick,            // SPACE: push every movable object in a random direction
    DeleteSelected,  // DELETE / BACKSPACE
    StepOnce,        // N: advance one physics step while paused
    ToggleUi         // F1: show / hide the sandbox panel
};

enum class MouseButton
{
    Left,
    Right
};

struct MouseButtonEvent
{
    bool pressed;  // true = down, false = up
    MouseButton button;
    int x;
    int y;
};

struct MousePosition
{
    int x;
    int y;
};

struct WindowSize
{
    int width;
    int height;
};

struct InputFrame
{
    std::vector<Command> commands;
    std::vector<MouseButtonEvent> buttons;    // In arrival order
    std::optional<MousePosition> mouseMoved;  // Last mouse position if it moved this frame
    std::optional<WindowSize> resized;
};

// Translates SDL events into game-level input. Every raw event is first offered to `uiHook`
// (e.g. the ImGui layer); if it returns true the event is consumed and not used by the game.
class Input
{
  public:
    using EventHook = std::function<bool(const SDL_Event&)>;

    InputFrame poll(const EventHook& uiHook = {}) const;
};
}  // namespace game
