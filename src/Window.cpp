#include "Window.hpp"

#include <stdexcept>

namespace game::sdl
{
Window::Window(const std::string& title, int width, int height, bool vsync)
    : m_width(width), m_height(height)
{
    m_window.reset(SDL_CreateWindow(title.c_str(), SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED,
                                    width, height, SDL_WINDOW_RESIZABLE));
    if (!m_window)
        throw std::runtime_error(std::string("Failed to create window: ") + SDL_GetError());

    Uint32 flags = SDL_RENDERER_ACCELERATED;
    if (vsync)
        flags |= SDL_RENDERER_PRESENTVSYNC;
    m_renderer.reset(SDL_CreateRenderer(m_window.get(), -1, flags));
    if (!m_renderer)
    {
        m_logger.warning("Accelerated renderer unavailable ({}), falling back to software.",
                         SDL_GetError());
        m_renderer.reset(SDL_CreateRenderer(m_window.get(), -1, SDL_RENDERER_SOFTWARE));
    }
    if (!m_renderer)
        throw std::runtime_error(std::string("Failed to create renderer: ") + SDL_GetError());

    SDL_GetWindowSize(m_window.get(), &m_width, &m_height);
    m_logger.info("Window created ({}x{}, vsync {})", m_width, m_height, vsync);
}

void Window::onResize(int width, int height)
{
    m_width = width;
    m_height = height;
}
}  // namespace game::sdl
