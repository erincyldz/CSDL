#pragma once
#include "ClassLogger.hpp"

class TestClass
{
  public:
    TestClass() : logger("TestClass") {}

    void doSomething();

  private:
    ClassLogger logger;
};