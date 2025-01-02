
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
    game::sdl::SDLHelper sdl("Test", WINDOW_WIDTH, WINDOW_HEIGHT, sdl_logger);
    game::object::CircleObject c(3, sdl_logger);
    game::object::RectObject r(3, 5, sdl_logger);
    std::cout << r.get_object_count() << std::endl;
    sdl.run();
}