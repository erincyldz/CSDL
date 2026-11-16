#include "Audio.hpp"

#include <algorithm>

namespace game::sdl
{
Audio::Audio(bool enabled, int volumePercent, const std::string& collisionSoundPath,
             const std::string& musicPath)
    : m_enabled(enabled)
{
    if (SDL_InitSubSystem(SDL_INIT_AUDIO) != 0)
    {
        m_logger.warning("Audio disabled: SDL audio init failed ({})", SDL_GetError());
        return;
    }
    if (!musicPath.empty())
        Mix_Init(MIX_INIT_MP3 | MIX_INIT_OGG | MIX_INIT_FLAC);  // WAV needs no decoder.

    if (Mix_OpenAudio(MIX_DEFAULT_FREQUENCY, MIX_DEFAULT_FORMAT, 2, 2048) != 0)
    {
        m_logger.warning("Audio disabled: Mix_OpenAudio failed ({})", Mix_GetError());
        Mix_Quit();
        SDL_QuitSubSystem(SDL_INIT_AUDIO);
        return;
    }
    m_available = true;

    m_collision.reset(Mix_LoadWAV(collisionSoundPath.c_str()));
    if (!m_collision)
        m_logger.warning("Failed to load '{}': {}", collisionSoundPath, Mix_GetError());

    if (!musicPath.empty())
    {
        m_music.reset(Mix_LoadMUS(musicPath.c_str()));
        if (!m_music)
            m_logger.warning("Failed to load music '{}': {}", musicPath, Mix_GetError());
    }

    setVolume(volumePercent);
    setEnabled(enabled);
}

Audio::~Audio()
{
    if (!m_available)
        return;
    Mix_HaltChannel(-1);
    Mix_HaltMusic();
    m_collision.reset();
    m_music.reset();
    Mix_CloseAudio();
    Mix_Quit();
    SDL_QuitSubSystem(SDL_INIT_AUDIO);
}

void Audio::setEnabled(bool enabled)
{
    m_enabled = enabled;
    if (!m_available)
        return;
    if (!enabled)
    {
        Mix_HaltChannel(-1);
        if (Mix_PlayingMusic())
            Mix_PauseMusic();
        return;
    }
    if (m_music)
    {
        if (!Mix_PlayingMusic())
            Mix_PlayMusic(m_music.get(), -1);
        else if (Mix_PausedMusic())
            Mix_ResumeMusic();
    }
}

void Audio::setVolume(int percent)
{
    m_volume = MIX_MAX_VOLUME * std::clamp(percent, 0, 100) / 100;
    if (!m_available)
        return;
    Mix_Volume(-1, m_volume);  // All effect channels.
    Mix_VolumeMusic(m_volume);
}

void Audio::playCollision()
{
    if (!m_available || !m_enabled || !m_collision)
        return;
    const Uint32 now = SDL_GetTicks();
    if (now - m_lastCollisionTick < MIN_COLLISION_INTERVAL_MS)
        return;
    m_lastCollisionTick = now;
    Mix_PlayChannel(-1, m_collision.get(), 0);
}
}  // namespace game::sdl
