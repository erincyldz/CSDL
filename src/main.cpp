#include "Config.hpp"
#include "Game.hpp"
#include "Headless.hpp"
#include "Logger.hpp"
#include "Platform.hpp"

#include <exception>
#include <iostream>

int main(int argc, char* argv[])
{
    game::platform::enableConsoleColors();

    game::Config config;
    switch (game::parseArgs(config, argc, argv, std::cerr))
    {
        case game::ParseResult::Ok:
            break;
        case game::ParseResult::ExitSuccess:
            return 0;
        case game::ParseResult::ExitFailure:
            return 1;
    }

    auto& logger = utils::Logger::getInstance();
    logger.setLogLevel(*utils::Logger::parseLevel(config.logLevel));

    try
    {
        // Headless runs never touch SDL, so they work without a display or sound device.
        if (config.headless)
            return game::headless::run(config, std::cout, std::cerr);

        game::Game game(config);
        game.run();
    }
    catch (const std::exception& e)
    {
        logger.log("main", utils::Logger::LogLevel::ERROR, "Fatal error: {}", e.what());
        return 1;
    }
    return 0;
}
