
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
    // game.addRandomGameObject(15);
    std::string logger = "Circle";
    auto circle1 = std::make_unique<game::object::CircleObject>(5, logger);
    auto circle2 = std::make_unique<game::object::CircleObject>(5, logger);
    game::object::Position pos1 = {game.getWindowWidth() / 2 - 100, game.getWindowHeight() / 2};
    game::object::Position pos2 = {game.getWindowWidth() / 2, game.getWindowHeight() / 2 + 100};
    circle1->setPosition(pos1);
    circle2->setPosition(pos2);

    game::object::Velocity vel1 = {50.0f, 0.0f};
    game::object::Velocity vel2 = {0.0f, -50.0f};
    circle1->setVelocity(vel1);
    circle2->setVelocity(vel2);

    game.addGameObject(std::move(circle1));
    game.addGameObject(std::move(circle2));

    game.run();
}