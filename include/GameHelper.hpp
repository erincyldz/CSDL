#pragma once
namespace game
{
#define MAX_GAME_OBJECTS      10
#define CALCULATE_GRAVITATION true
#define BORDER_COLLISION      true
#define RENDER_LAST_POSITIONS true
#define RENDER_DIRECTION      true

enum class GameState
{
    MENU,
    SETTINGS,
    PLAYING,
    EXIT,
    PAUSED
};

}  // namespace game