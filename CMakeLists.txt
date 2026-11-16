cmake_minimum_required(VERSION 3.16)

# Set the project name and version
project(engine VERSION 1.0 LANGUAGES CXX)

# Specify the C++ standard
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED True)
set(CMAKE_CXX_EXTENSIONS OFF)

# Default to an optimized build, so every machine gets the same result from a plain `cmake ..`
# (Debug: cmake -DCMAKE_BUILD_TYPE=Debug ..). Multi-config generators (Visual Studio) pick it per build.
if(NOT CMAKE_BUILD_TYPE AND NOT CMAKE_CONFIGURATION_TYPES)
    set(CMAKE_BUILD_TYPE Release CACHE STRING "Build type" FORCE)
    set_property(CACHE CMAKE_BUILD_TYPE PROPERTY STRINGS Debug Release RelWithDebInfo MinSizeRel)
endif()
if(NOT CMAKE_CONFIGURATION_TYPES)
    message(STATUS "CSDL build type: ${CMAKE_BUILD_TYPE}")
endif()

option(CSDL_WERROR "Treat compiler warnings as errors (enabled in CI)" OFF)

# Put the executable in <project root>/bin (override with -DCMAKE_RUNTIME_OUTPUT_DIRECTORY=...)
if(NOT CMAKE_RUNTIME_OUTPUT_DIRECTORY)
    set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_SOURCE_DIR}/bin)
endif()

# Dependencies. Each package provides an imported target that carries its own include
# directories and link flags, so nothing here depends on where the libraries are installed
# (apt, a source build in /usr/local, vcpkg, ...). Use -DCMAKE_PREFIX_PATH=... for other prefixes.
find_package(SDL2 REQUIRED)
find_package(SDL2_mixer REQUIRED)
find_package(SDL2_ttf REQUIRED)
message(STATUS "SDL2: ${SDL2_DIR}  SDL2_mixer: ${SDL2_mixer_DIR}  SDL2_ttf: ${SDL2_ttf_DIR}")

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

if(TARGET SDL2::SDL2main)
    target_link_libraries(${PROJECT_NAME} PRIVATE SDL2::SDL2main)
endif()
target_link_libraries(${PROJECT_NAME} PRIVATE
    SDL2::SDL2
    SDL2_mixer::SDL2_mixer
    SDL2_ttf::SDL2_ttf
    imgui
    $<$<PLATFORM_ID:Linux>:m>)
target_include_directories(${PROJECT_NAME} PRIVATE include)

# Copy assets next to the executable so it can be run from anywhere (found via SDL_GetBasePath).
add_custom_command(TARGET ${PROJECT_NAME} POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E copy_directory
            ${CMAKE_SOURCE_DIR}/assets $<TARGET_FILE_DIR:${PROJECT_NAME}>/assets)

# Warnings for our own code (not for ImGui). They only stop the build with -DCSDL_WERROR=ON, so a
# newer compiler with new warnings doesn't break someone's local build; CI keeps the code clean.
# Optimization flags come from the build type (Release: -O3 -DNDEBUG, Debug: -g).
function(csdl_set_warnings target)
    if (CMAKE_CXX_COMPILER_ID MATCHES "GNU|Clang")
        target_compile_options(${target} PRIVATE -Wall -Wextra -pedantic $<$<BOOL:${CSDL_WERROR}>:-Werror>)
    elseif (CMAKE_CXX_COMPILER_ID MATCHES "MSVC")
        target_compile_options(${target} PRIVATE /W4 $<$<BOOL:${CSDL_WERROR}>:/WX>)
    endif()
endfunction()
csdl_set_warnings(${PROJECT_NAME})

# ---------------------------------------------------------------------------
# Micro benchmarks (standalone, no SDL): cmake -DCSDL_BUILD_BENCHMARKS=ON .. && ./bin/csdl_bench
# ---------------------------------------------------------------------------
option(CSDL_BUILD_BENCHMARKS "Build the micro benchmarks in bench/" OFF)
if(CSDL_BUILD_BENCHMARKS)
    add_executable(csdl_bench bench/optimization_bench.cpp)
    csdl_set_warnings(csdl_bench)
endif()
