#include "Config.hpp"
#include "Headless.hpp"
#include "Logger.hpp"
#include "Platform.hpp"

#include <exception>
#include <iostream>

// Entry point of csdl_headless: the headless runner without any SDL code, for machines that have
// no SDL installed (servers, CI, a Raspberry Pi over SSH). It takes the same options as
// `engine --headless`; --headless itself is implied.
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
    config.headless = true;

    auto& logger = utils::Logger::getInstance();
    logger.setLogLevel(*utils::Logger::parseLevel(config.logLevel));

    try
    {
        return game::headless::run(config, std::cout, std::cerr);
    }
    catch (const std::exception& e)
    {
        logger.log("main", utils::Logger::LogLevel::ERROR, "Fatal error: {}", e.what());
        return 1;
    }
}
