#pragma once
#include <SDL2/SDL.h>
#include <SDL2/SDL_mixer.h>
#include <SDL2/SDL_ttf.h>
#include <memory>
#include <stdexcept>
#include <string>

// unique_ptr aliases with custom deleters so SDL resources are released automatically.
namespace game::sdl
{
struct WindowDeleter
{
    void operator()(SDL_Window* p) const noexcept
    {
        SDL_DestroyWindow(p);
    }
};
struct RendererDeleter
{
    void operator()(SDL_Renderer* p) const noexcept
    {
        SDL_DestroyRenderer(p);
    }
};
struct TextureDeleter
{
    void operator()(SDL_Texture* p) const noexcept
    {
        SDL_DestroyTexture(p);
    }
};
struct SurfaceDeleter
{
    void operator()(SDL_Surface* p) const noexcept
    {
        SDL_FreeSurface(p);
    }
};
struct FontDeleter
{
    void operator()(TTF_Font* p) const noexcept
    {
        TTF_CloseFont(p);
    }
};
struct ChunkDeleter
{
    void operator()(Mix_Chunk* p) const noexcept
    {
        Mix_FreeChunk(p);
    }
};
struct MusicDeleter
{
    void operator()(Mix_Music* p) const noexcept
    {
        Mix_FreeMusic(p);
    }
};

using WindowPtr = std::unique_ptr<SDL_Window, WindowDeleter>;
using RendererPtr = std::unique_ptr<SDL_Renderer, RendererDeleter>;
using TexturePtr = std::unique_ptr<SDL_Texture, TextureDeleter>;
using SurfacePtr = std::unique_ptr<SDL_Surface, SurfaceDeleter>;
using FontPtr = std::unique_ptr<TTF_Font, FontDeleter>;
using ChunkPtr = std::unique_ptr<Mix_Chunk, ChunkDeleter>;
using MusicPtr = std::unique_ptr<Mix_Music, MusicDeleter>;

// Owns SDL video and SDL_ttf initialization for its lifetime. Must outlive every other SDL
// object, so declare it first in the owning class. (Audio initializes its own subsystem.)
class SdlContext
{
  public:
    SdlContext()
    {
        if (SDL_Init(SDL_INIT_VIDEO) != 0)
            throw std::runtime_error(std::string("SDL_Init failed: ") + SDL_GetError());
        if (TTF_Init() != 0)
        {
            SDL_Quit();
            throw std::runtime_error(std::string("TTF_Init failed: ") + TTF_GetError());
        }
    }
    ~SdlContext()
    {
        TTF_Quit();
        SDL_Quit();
    }
    SdlContext(const SdlContext&) = delete;
    SdlContext& operator=(const SdlContext&) = delete;
};
}  // namespace game::sdl
