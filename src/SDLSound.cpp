#include "SDLSound.hpp"

namespace game::sdl::sound
{
Sound::Sound(std::string filepath)
{
    // if (SDL_LoadWAV(filepath.c_str(), &m_audioSpec, &m_waveStart, &m_waveLength) == nullptr)
    // {
    //     std::cerr << "sound loading error" << SDL_GetError() << std::endl;
    // }
}
Sound::~Sound()
{
    SDL_FreeWAV(m_waveStart);
    SDL_CloseAudioDevice(m_device);
}
void Sound::PlaySound()
{
    int status = SDL_QueueAudio(m_device, m_waveStart, m_waveLength);
    SDL_PauseAudioDevice(m_device, 0);
}
void Sound::StopSound()
{
    SDL_PauseAudioDevice(m_device, 1);
}
void Sound::SetupDevice()
{
    m_device = SDL_OpenAudioDevice(nullptr, 0, &m_audioSpec, nullptr, SDL_AUDIO_ALLOW_ANY_CHANGE);
    if (0 == m_device)
    {
        std::cerr << "Sound device error:" << SDL_GetError() << std::endl;
    }
}

void Sound::ClearSoundQueue()
{
    SDL_ClearQueuedAudio(m_device);
}

int Sound::loadMusic(std::string filename)
{
    Mix_Music* m = NULL;
    m = Mix_LoadMUS(filename.c_str());
    if (m == NULL)
    {
        std::cerr << "Failed to load music." << Mix_GetError() << std::endl;
    }
    music.push_back(m);
    return music.size() - 1;
}

int Sound::loadSound(std::string filename)
{
    Mix_Chunk* s = NULL;
    s = Mix_LoadWAV(filename.c_str());
    if (s == NULL)
    {
        std::cerr << "Failed to load music." << Mix_GetError() << std::endl;
    }
    sounds.push_back(s);
    return sounds.size() - 1;
}

void Sound::setVolume(int v)
{
    volume = (MIX_MAX_VOLUME * v) / 100;
}

int Sound::playMusic(int m)
{
    if (!Mix_PlayingMusic())
    {
        Mix_Volume(1, volume);
        Mix_PlayMusic(music[m], -1);
    }
}

int Sound::playSound()
{
    Mix_Volume(-1, volume);
    Mix_PlayChannel(-1, sounds[sound_collision], 0);
    return 0;
}

void Sound::initMixer()
{
    Mix_Init(MIX_INIT_MP3);
    if (Mix_OpenAudio(44100, MIX_DEFAULT_FORMAT, 2, 2048) < 0)
    {
        std::cerr << "Fucked up openAudio" << Mix_GetError() << std::endl;
    }
    setVolume(80);
    sound_collision = loadSound("./soundfiles/8bitCollision.wav");
    return;
}
void Sound::quitMixer()
{
    for (int s = 0; s < sounds.size(); s++)
    {
        Mix_FreeChunk(sounds[s]);
        sounds[s] = NULL;
    }
    for (int m = 0; m < music.size(); m++)
    {
        Mix_FreeMusic(music[m]);
        music[m] = NULL;
    }
    Mix_Quit();
}
}  // namespace game::sdl::sound