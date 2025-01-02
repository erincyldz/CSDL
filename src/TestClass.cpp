#include "TestClass.hpp"

void TestClass::doSomething()
{
    int my_val = 50;
    logger.error("Error Value is {}", my_val);
    logger.warning("Warning Value is {}", my_val);
    logger.info("Info Value is {}", my_val);
    logger.debug("Info Value Debug is {}", my_val);
}