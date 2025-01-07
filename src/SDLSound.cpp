#include "SDLSound.hpp"

namespace game::sdl::sound
{
Sound::Sound()
{
    Mix_Init(MIX_INIT_MP3);
    if (Mix_OpenAudio(44100, MIX_DEFAULT_FORMAT, 2, 2048) < 0)
    {
        std::cerr << "Fucked up openAudio" << Mix_GetError() << std::endl;
    }
    setVolume(10);
    sound_collision = loadSound("./assets/sounds/8bitCollision.wav");
    return;
}
Sound::~Sound()
{
    quitMixer();
}

int Sound::loadMusic(std::string filename)
{
    std::string full_path = std::filesystem::path(PROJECT_ROOT_DIR) / filename;
    Mix_Music* m = NULL;
    m = Mix_LoadMUS(full_path.c_str());
    if (m == NULL)
    {
        std::cerr << "Failed to load music." << Mix_GetError() << std::endl;
    }
    music.push_back(m);
    return music.size() - 1;
}

int Sound::loadSound(std::string filename)
{
    std::string full_path = std::filesystem::path(PROJECT_ROOT_DIR) / filename;

    Mix_Chunk* s = NULL;
    s = Mix_LoadWAV(full_path.c_str());
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

int Sound::playMusic()
{
    if (!Mix_PlayingMusic())
    {
        Mix_Volume(1, volume);
        Mix_PlayMusic(music[game_music], -1);
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
    Mix_Init(MIX_INIT_MP3 | MIX_INIT_FLAC);
    if (Mix_OpenAudio(MIX_DEFAULT_FREQUENCY, MIX_DEFAULT_FORMAT, 2, 8192) < 0)
    {
        std::cerr << "Fucked up openAudio" << Mix_GetError() << std::endl;
    }
    setVolume(10);
    sound_collision = loadSound("assets/sounds/8bitCollision.wav");
    game_music = loadMusic("assets/sounds/rhcp-snow.mp3");
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