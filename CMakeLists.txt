cmake_minimum_required(VERSION 3.16)

# Set the project name and version
project(engine VERSION 1.0 LANGUAGES CXX)

# Specify the C++ standard
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED True)

# Put the executable in <project root>/bin (override with -DCMAKE_RUNTIME_OUTPUT_DIRECTORY=...)
if(NOT CMAKE_RUNTIME_OUTPUT_DIRECTORY)
    set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_SOURCE_DIR}/bin)
endif()

# Set the prefix path to include /usr/local for package searches
set(CMAKE_PREFIX_PATH "/usr/local" ${CMAKE_PREFIX_PATH})

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

find_package(SDL2_ttf REQUIRED)
if(SDL2_ttf_FOUND)
    message(STATUS "SDL2_ttf found at: ${SDL2_ttf_DIR}")
else()
    message(FATAL_ERROR "SDL2_ttf not found!")
endif()

# ---------------------------------------------------------------------------
# Dear ImGui (sandbox UI). Downloaded at configure time with a pinned version and checksum.
# For offline builds pass -DCSDL_IMGUI_DIR=/path/to/imgui-1.92.9b
# ---------------------------------------------------------------------------
set(CSDL_IMGUI_DIR "" CACHE PATH "Use a local Dear ImGui source tree instead of downloading it")
if(CSDL_IMGUI_DIR)
    set(IMGUI_SOURCE_DIR ${CSDL_IMGUI_DIR})
else()
    include(FetchContent)
    if(POLICY CMP0135)
        cmake_policy(SET CMP0135 NEW)
    endif()
    FetchContent_Declare(imgui
        URL https://github.com/ocornut/imgui/archive/refs/tags/v1.92.9b.tar.gz
        URL_HASH SHA256=21d8a0a565e85dce943e375db00812c2f3f0ab21f3f0f7964e364a63422d7f99)
    FetchContent_MakeAvailable(imgui)
    set(IMGUI_SOURCE_DIR ${imgui_SOURCE_DIR})
endif()

add_library(imgui STATIC
    ${IMGUI_SOURCE_DIR}/imgui.cpp
    ${IMGUI_SOURCE_DIR}/imgui_draw.cpp
    ${IMGUI_SOURCE_DIR}/imgui_tables.cpp
    ${IMGUI_SOURCE_DIR}/imgui_widgets.cpp
    ${IMGUI_SOURCE_DIR}/imgui_demo.cpp
    ${IMGUI_SOURCE_DIR}/backends/imgui_impl_sdl2.cpp
    ${IMGUI_SOURCE_DIR}/backends/imgui_impl_sdlrenderer2.cpp)
target_include_directories(imgui SYSTEM PUBLIC ${IMGUI_SOURCE_DIR} ${IMGUI_SOURCE_DIR}/backends)
target_link_libraries(imgui PUBLIC SDL2::SDL2)

# ---------------------------------------------------------------------------
# Game executable
# ---------------------------------------------------------------------------
file(GLOB SOURCES src/*.cpp)
add_executable(${PROJECT_NAME} ${SOURCES})

target_link_libraries(${PROJECT_NAME} PRIVATE SDL2::SDL2 SDL2::SDL2main SDL2_mixer SDL2_ttf imgui m)
target_include_directories(${PROJECT_NAME} PRIVATE include)

# Copy assets next to the executable so it can be run from anywhere (found via SDL_GetBasePath).
add_custom_command(TARGET ${PROJECT_NAME} POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E copy_directory
            ${CMAKE_SOURCE_DIR}/assets $<TARGET_FILE_DIR:${PROJECT_NAME}>/assets)

# Enable warnings and treat warnings as errors (only for our own code, not for ImGui)
if (CMAKE_CXX_COMPILER_ID MATCHES "GNU|Clang")
    target_compile_options(${PROJECT_NAME} PRIVATE -Wall -Wextra -Werror -pedantic)
    target_compile_options(${PROJECT_NAME} PRIVATE $<$<CONFIG:Release>:-O2> $<$<CONFIG:Debug>:-O0 -g>)
elseif (CMAKE_CXX_COMPILER_ID MATCHES "MSVC")
    target_compile_options(${PROJECT_NAME} PRIVATE /W4 /WX)
endif()
