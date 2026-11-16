#pragma once
#include <SDL2/SDL.h>

namespace game::ui
{
// Owns the Dear ImGui context and its SDL2 + SDL_Renderer backends. Must be destroyed before
// the SDL renderer/window it was created with.
class ImGuiLayer
{
  public:
    ImGuiLayer(SDL_Window* window, SDL_Renderer* renderer, float scale);
    ~ImGuiLayer();
    ImGuiLayer(const ImGuiLayer&) = delete;
    ImGuiLayer& operator=(const ImGuiLayer&) = delete;

    // Feeds an event to ImGui. Returns true if ImGui wants it exclusively (cursor over a panel,
    // text field focused, ...), in which case the game should ignore it.
    bool processEvent(const SDL_Event& event);

    void beginFrame();
    // True while the cursor is over a panel (the world shouldn't show hover feedback then).
    bool wantsMouse() const;
    // Finishes the ImGui frame and draws it with the given renderer (call before present).
    void render(SDL_Renderer* renderer);

  private:
    bool m_frameStarted = false;
};
}  // namespace game::ui
