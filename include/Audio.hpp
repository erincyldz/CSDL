#pragma once
#include "ClassLogger.hpp"
#include "SdlRaii.hpp"

#include <string>

namespace game::sdl
{
// Owns the SDL audio subsystem and SDL_mixer. If audio can't be opened (e.g. no sound device)
// the game keeps running silently.
class Audio
{
  public:
    Audio(bool enabled, int volumePercent, const std::string& collisionSoundPath,
          const std::string& musicPath);
    ~Audio();
    Audio(const Audio&) = delete;
    Audio& operator=(const Audio&) = delete;

    bool isAvailable() const
    {
        return m_available;
    }
    bool isEnabled() const
    {
        return m_enabled;
    }
    void setEnabled(bool enabled);
    void setVolume(int percent);

    // Plays the collision effect; rate-limited so overlapping contacts don't spam it.
    void playCollision();

  private:
    static constexpr Uint32 MIN_COLLISION_INTERVAL_MS = 60;

    ClassLogger m_logger{"Audio"};
    bool m_available = false;
    bool m_enabled;
    int m_volume = 0;  // SDL_mixer scale 0..MIX_MAX_VOLUME
    ChunkPtr m_collision;
    MusicPtr m_music;
    Uint32 m_lastCollisionTick = 0;
};
}  // namespace game::sdl
