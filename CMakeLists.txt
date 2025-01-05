cmake_minimum_required(VERSION 3.10)

# Set the project name and version
project(engine VERSION 1.0 LANGUAGES CXX)

# Specify the C++ standard
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED True)

# Set the output directory for build artifacts
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR})

# Set the prefix path to include /usr/local for package searches
set(CMAKE_PREFIX_PATH "/usr/local" ${CMAKE_PREFIX_PATH})
# Include directories for the project

# Automatically add all .cpp files in the src directory
file(GLOB SOURCES src/*.cpp)

# Add the executable
add_executable(${PROJECT_NAME} ${SOURCES})

# Find and link SDL2 and SDL2_mixer libraries
find_package(SDL2 REQUIRED)
if(SDL2_FOUND)
    message(STATUS "SDL2 found at: ${SDL2_DIR}")
else()
    message(FATAL_ERROR "SDL2 not found!")
endif()

find_package(SDL2_mixer REQUIRED)
if(SDL2_mixer_FOUND)
    message(STATUS "SDL2_mixer found at: ${SDL2_mixer_DIR}")
else()
    message(FATAL_ERROR "SDL2_mixer not found!")
endif()


target_link_libraries(${PROJECT_NAME} PRIVATE SDL2::SDL2 SDL2::SDL2main SDL2_mixer m)

# Add include directories for the target (optional, more scoped)
target_include_directories(${PROJECT_NAME} PRIVATE include)
