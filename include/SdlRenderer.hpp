#pragma once
#include "ClassLogger.hpp"
#include "IRenderer.hpp"
#include "SdlRaii.hpp"
#include "Window.hpp"

#include <string>
#include <unordered_map>

namespace game::sdl
{
class SdlRenderer : public IRenderer
{
  public:
    SdlRenderer(Window& window, const std::string& fontPath, int fontSize = 24);

    void beginFrame() override;
    void drawWorld(const physics::PhysicsWorld& world, const RenderOptions& options) override;
    void drawHud(const HudInfo& hud) override;
    void drawSandboxOverlay(const physics::PhysicsWorld& world,
                            const SandboxOverlay& overlay) override;
    void drawMenu(const ui::Menu& menu) override;
    void drawPausedOverlay(int viewWidth) override;
    void endFrame() override;

  private:
    struct CachedText
    {
        TexturePtr texture;
        int width = 0;
        int height = 0;
    };

    // Rendered text textures are cached; rebuilding them every frame is expensive.
    const CachedText* text(const std::string& str, SDL_Color color, bool small = false);
    void drawText(const std::string& str, int x, int y, SDL_Color color, bool small = false);
    void drawTextCentered(const std::string& str, int centerX, int centerY, SDL_Color color);

    void setColor(SDL_Color color);
    void fillCircle(double cx, double cy, int radius);
    void ringOutline(double cx, double cy, int radius, int thickness);
    void rectOutline(double left, double top, int width, int height, int thickness);
    void drawObject(const object::GameObject& obj);
    void drawShapeOutline(const object::Shape& shape, const math::Vector2D& center, int thickness);
    void drawTrail(const object::GameObject& obj);
    void drawDirection(const object::GameObject& obj);

    static constexpr std::size_t MAX_CACHED_TEXTS = 128;

    ClassLogger m_logger{"SdlRenderer"};
    Window& m_window;
    SDL_Renderer* m_renderer;
    FontPtr m_font;
    FontPtr m_smallFont;
    std::unordered_map<std::string, CachedText> m_textCache;
};
}  // namespace game::sdl
