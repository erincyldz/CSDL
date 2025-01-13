
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
    auto circle3 = std::make_unique<game::object::CircleObject>(15, logger);
    float width = game.getWindowWidth();
    float height = game.getWindowHeight();
    game::object::Position pos1 = {width / 2, height / 2 - 50};
    game::object::Position pos2 = {width / 2, height / 2 - 100};
    game::object::Position pos3 = {width / 2, height / 2 - 150};
    circle1->setPosition(pos1);
    circle2->setPosition(pos2);
    circle3->setPosition(pos3);

    circle1->setMass(1.0);
    circle2->setMass(1.0);
    circle3->setMass(1.0);

    game::object::Velocity vel1 = {30.0f, 30.0f};
    game::object::Velocity vel2 = {-30.0f, -30.0f};
    game::object::Velocity vel3 = {30.0f, 30.0f};
    circle1->setVelocity(vel1);
    circle2->setVelocity(vel2);
    circle3->setVelocity(vel3);

    game.addGameObject(std::move(circle1));
    game.addGameObject(std::move(circle2));
    game.addGameObject(std::move(circle3));

    game.run();
}