
#include <CircleObject.hpp>
#include <Game.hpp>
#include <RectObject.hpp>
#include <SDLHelper.hpp>
#include <iostream>

int main(int argc, char* argv[])
{
    if (argc == 1)
    {
        std::cout << "No logger level specified, logger will be initialized as INFO level.\n";
    }
    else
    {
        utils::Logger::getInstance().setLogLevel(
            utils::Logger::getInstance().parseLogLevel(argc, argv));
    }

    game::Game game;
    std::string loggerName = "CircleLogger";
    game.run();
}