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
    std::string logger1 = "Circle1";
    std::string logger2 = "Circle2";
    std::string logger3 = "Circle3";
    std::string logger4 = "Rect1";
    auto circle1 = std::make_unique<game::object::CircleObject>(5, logger1);
    auto circle2 = std::make_unique<game::object::CircleObject>(15, logger2);
    auto circle3 = std::make_unique<game::object::CircleObject>(15, logger3);
    auto rectan1 = std::make_unique<game::object::RectObject>(logger1);
    float width = game.getWindowWidth();
    float height = game.getWindowHeight();
    game::object::Position pos1 = {width / 2 - 250, height / 2};
    game::object::Position pos2 = {width / 2 + 250, height / 2};
    game::object::Position pos3 = {width / 2, height / 2 - 200};
    game::object::Position pos4 = {width / 2 - 300, height / 2 + 70};

    circle1->setPosition(pos1);
    circle2->setPosition(pos2);
    circle3->setPosition(pos3);
    rectan1->setPosition(pos4);

    circle1->setMass(1.0);
    circle2->setMass(5000.0);
    circle3->setMass(100.0);
    rectan1->setMass(50.0);

    circle1->setRestitution(1.0f);
    circle2->setRestitution(1.0f);
    circle3->setRestitution(1.0f);
    rectan1->setRestitution(1.0f);

    game::object::Velocity vel1 = {0.0f, 0.0f};
    game::object::Velocity vel2 = {0.0f, 0.0f};
    game::object::Velocity vel3 = {0.0f, 0.0f};
    game::object::Velocity vel4 = {0.0f, 0.0f};
    circle1->setVelocity(vel1);
    circle2->setVelocity(vel2);
    circle3->setVelocity(vel3);
    rectan1->setVelocity(vel4);

    game.addGameObject(std::move(circle1));
    game.addGameObject(std::move(circle2));
    game.addGameObject(std::move(circle3));
    // game.addGameObject(std::move(rectan1));

    game.run();
}