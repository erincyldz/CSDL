#include <SDL2/SDL.h>
#include <SDL_mixer.h>
#include <iostream>
#include <string>
#include <vector>
#ifndef SDL_SOUND_H
#define SDL_SOUND_H
class SDLHelper;

namespace game::sdl::sound
{
// Interface Class for Sound class
class ISound
{
    friend class SDLHelper;

  public:
    virtual ~ISound() {};

    virtual void PlaySound() = 0;
    virtual void StopSound() = 0;
};

class Sound : public ISound
{
    friend class SDLHelper;

  public:
    Sound(std::string filepath);
    ~Sound();
    void PlaySound();
    void StopSound();
    void SetupDevice();
    void ClearSoundQueue();
    // Mixer Spesific methods.
    void initMixer();
    void quitMixer();
    int loadMusic(std::string musicPath);
    int loadSound(std::string soundPath);
    void setVolume(int v);
    int playMusic(int m);
    int playSound();

  private:
    int volume;
    int sound_collision;
    SDL_AudioDeviceID m_device;
    std::vector<Mix_Chunk*> sounds;
    std::vector<Mix_Music*> music;
    SDL_AudioSpec m_audioSpec;
    uint8_t* m_waveStart;
    uint32_t m_waveLength;
};
}  // namespace game::sdl::sound
#endif