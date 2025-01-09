
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
    std::string loggerr = "Rect";
    auto circle1 = std::make_unique<game::object::CircleObject>(15, logger);
    auto circle2 = std::make_unique<game::object::CircleObject>(15, logger);
    auto rect = std::make_unique<game::object::RectObject>(loggerr);
    float width = game.getWindowWidth();
    float height = game.getWindowHeight();
    game::object::Position pos1 = {width / 2 - 100, height / 2};
    game::object::Position pos2 = {width / 2, height / 2 + 300};
    game::object::Position pos3 = {width / 2, height / 2 - 300};
    circle1->setPosition(pos1);
    circle2->setPosition(pos2);
    rect->setPosition(pos3);
    circle1->setRestitution(0.0);
    circle2->setRestitution(0.0);
    rect->setRestitution(0.0);

    game::object::Velocity vel1 = {0.0f, 0.0f};
    game::object::Velocity vel2 = {0.0f, -50.0f};
    game::object::Velocity vel3 = {10.0f, 0.0f};
    circle1->setVelocity(vel1);
    circle2->setVelocity(vel2);
    rect->setVelocity(vel3);

    game.addGameObject(std::move(circle1));
    game.addGameObject(std::move(circle2));
    game.addGameObject(std::move(rect));

    game.run();
}