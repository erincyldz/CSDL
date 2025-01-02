cmake_minimum_required(VERSION 3.10)

# Set the project name and version
project(engine VERSION 1.0 LANGUAGES CXX)

# Specify the C++ standard
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED True)

# Set the output directory for build artifacts
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR})

# Include directories
include_directories(include)

# Automatically add all .cpp files in the src directory
file(GLOB SOURCES src/*.cpp)

# Add the executable
add_executable(${PROJECT_NAME} ${SOURCES})

# Link the SDL2 library and math library
find_package(SDL2 REQUIRED)
target_link_libraries(${PROJECT_NAME} SDL2::SDL2 SDL2::SDL2main m)
