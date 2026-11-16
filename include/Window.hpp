#pragma once
#include "ClassLogger.hpp"
#include "SdlRaii.hpp"

#include <string>

namespace game::sdl
{
// SDL window + accelerated renderer.
class Window
{
  public:
    Window(const std::string& title, int width, int height, bool vsync);

    SDL_Window* handle() const
    {
        return m_window.get();
    }
    SDL_Renderer* renderer() const
    {
        return m_renderer.get();
    }
    int width() const
    {
        return m_width;
    }
    int height() const
    {
        return m_height;
    }
    void onResize(int width, int height);

  private:
    ClassLogger m_logger{"Window"};
    WindowPtr m_window;
    RendererPtr m_renderer;
    int m_width;
    int m_height;
};
}  // namespace game::sdl
