cmake_minimum_required(VERSION 3.10)

# Set the project name and version
project(engine VERSION 1.0 LANGUAGES C)

# Specify the C standard
set(CMAKE_C_STANDARD 99)
set(CMAKE_C_STANDARD_REQUIRED True)

# Set the output directory for build artifacts
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR})

# Include directories
include_directories(include)

# Automatically add all .c files in the src directory
file(GLOB SOURCES src/*.c)

# Add the executable
add_executable(${PROJECT_NAME} ${SOURCES})

# Link the SDL2 library and math library
find_package(SDL2 REQUIRED)
target_link_libraries(${PROJECT_NAME} SDL2::SDL2 SDL2::SDL2main m)
