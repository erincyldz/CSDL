#include "Input.hpp"

#include <SDL2/SDL.h>

namespace game
{
namespace
{
std::optional<Command> commandForKey(SDL_Keycode key)
{
    switch (key)
    {
        case SDLK_ESCAPE:
            return Command::Back;
        case SDLK_p:
            return Command::TogglePause;
        case SDLK_r:
            return Command::Reset;
        case SDLK_q:
            return Command::SpawnRandom;
        case SDLK_SPACE:
            return Command::Kick;
        case SDLK_DELETE:
        case SDLK_BACKSPACE:
            return Command::DeleteSelected;
        case SDLK_n:
            return Command::StepOnce;
        case SDLK_F1:
            return Command::ToggleUi;
        default:
            return std::nullopt;
    }
}

std::optional<MouseButton> buttonFor(Uint8 sdlButton)
{
    if (sdlButton == SDL_BUTTON_LEFT)
        return MouseButton::Left;
    if (sdlButton == SDL_BUTTON_RIGHT)
        return MouseButton::Right;
    return std::nullopt;
}
}  // namespace

InputFrame Input::poll(const EventHook& uiHook) const
{
    InputFrame frame;
    SDL_Event event;
    while (SDL_PollEvent(&event))
    {
        // Quit and resize always reach the game; everything else may be consumed by the UI.
        if (event.type == SDL_QUIT)
        {
            frame.commands.push_back(Command::Quit);
            continue;
        }
        if (event.type == SDL_WINDOWEVENT)
        {
            if (uiHook)
                uiHook(event);
            if (event.window.event == SDL_WINDOWEVENT_SIZE_CHANGED)
                frame.resized = WindowSize{event.window.data1, event.window.data2};
            continue;
        }

        const bool consumedByUi = uiHook && uiHook(event);

        switch (event.type)
        {
            case SDL_KEYDOWN:
                if (consumedByUi || event.key.repeat)
                    break;
                if (const auto cmd = commandForKey(event.key.keysym.sym))
                    frame.commands.push_back(*cmd);
                break;

            case SDL_MOUSEBUTTONDOWN:
                if (consumedByUi)
                    break;
                if (const auto b = buttonFor(event.button.button))
                    frame.buttons.push_back(
                        MouseButtonEvent{true, *b, event.button.x, event.button.y});
                break;

            // Releases and motion are always delivered, so a drag that started in the world
            // ends correctly even if the cursor is over a panel when the button is released.
            case SDL_MOUSEBUTTONUP:
                if (const auto b = buttonFor(event.button.button))
                    frame.buttons.push_back(
                        MouseButtonEvent{false, *b, event.button.x, event.button.y});
                break;

            case SDL_MOUSEMOTION:
                frame.mouseMoved = MousePosition{event.motion.x, event.motion.y};
                break;

            default:
                break;
        }
    }
    return frame;
}
}  // namespace game
