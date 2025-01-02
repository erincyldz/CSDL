
#include <CircleObject.hpp>
#include <RectObject.hpp>
#include <SDLHelper.hpp>
#include <iostream>

constexpr int WINDOW_HEIGHT = 600;
constexpr int WINDOW_WIDTH = 800;
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
    std::string sdl_logger = "SDLLOGGER";
    sdl::SDLHelper sdl("Test", WINDOW_WIDTH, WINDOW_HEIGHT, sdl_logger);
    sdl.run();
}