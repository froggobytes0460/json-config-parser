# Project Configuration for CMake
cmake_minimum_required(VERSION 3.20)
project(
    json-config-parser
    VERSION 0.1.0
    LANGUAGES CXX
)

# Enforce C++20 standard
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

# Create the .gitignore instantly inside the build folder
if(NOT "${CMAKE_CURRENT_SOURCE_DIR}" STREQUAL "${CMAKE_CURRENT_BINARY_DIR}")
  file(WRITE "${CMAKE_CURRENT_BINARY_DIR}/.gitignore" "*\n")
endif()

# Install dependencies
set(FETCHCONTENT_BASE_DIR "${CMAKE_SOURCE_DIR}/dependencies" CACHE PATH "Custom FetchContent base directory")
include(FetchContent)

FetchContent_Declare(
    fmt
    GIT_REPOSITORY https://github.com/fmtlib/fmt.git
    GIT_TAG 11.0.2
    SYSTEM
)
FetchContent_Declare(
    argparse
    GIT_REPOSITORY https://github.com/p-ranav/argparse.git
    GIT_TAG v3.2
    SYSTEM
)
FetchContent_Declare(
    catch2
    GIT_REPOSITORY https://github.com/catchorg/Catch2.git
    GIT_TAG v3.5.0
    SYSTEM
)
FetchContent_Declare(
    nlohmann_json
    GIT_REPOSITORY https://github.com/nlohmann/json.git
    GIT_TAG v3.11.3
    SYSTEM
)

FetchContent_MakeAvailable(nlohmann_json argparse catch2 fmt)

# Create a library target project
add_library(json-config-lib STATIC
    include/config.hpp
    include/parser.hpp
    src/config.cpp
    src/parser.cpp
)

target_include_directories(json-config-lib PUBLIC include)
target_link_libraries(json-config-lib
    PUBLIC
        nlohmann_json::nlohmann_json
        argparse::argparse
        fmt::fmt
)

# =================================================================================
# Testing Configuration
# =================================================================================
enable_testing()

add_executable(json-config-parser-tests
    tests/test_config.cpp
)

target_link_libraries(json-config-parser-tests
    PRIVATE
        json-config-lib
        Catch2::Catch2WithMain
)

include(CTest)
include(Catch)
catch_discover_tests(json-config-parser-tests)
# =================================================================================

# Main executable target
add_executable(json-config-parser src/main.cpp)
target_link_libraries(json-config-parser PRIVATE json-config-lib)

# MacOS optimization.
if(APPLE)
    add_custom_command(TARGET json-config-parser POST_BUILD
        COMMAND codesign --force --sign - $<TARGET_FILE:json-config-parser>
        COMMENT "MACOS: Applying ad-hoc code signature to bypass Gatekeeper scan"
    )
endif()
