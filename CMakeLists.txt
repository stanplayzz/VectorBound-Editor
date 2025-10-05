cmake_minimum_required(VERSION 3.28)
project(CMakeSFMLProject LANGUAGES CXX)

set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)

include(FetchContent)

# sfml
FetchContent_Declare(SFML
    GIT_REPOSITORY https://github.com/SFML/SFML.git
    GIT_TAG 3.0.1
    GIT_SHALLOW ON
    EXCLUDE_FROM_ALL
    SYSTEM)

FetchContent_MakeAvailable(SFML)

# djson
FetchContent_Declare(
        djson
        GIT_REPOSITORY https://github.com/karnkaul/djson
        GIT_TAG v3.1.1
)
FetchContent_MakeAvailable(djson)

# tiny file dialogs
add_library(
    tinyfiledialogs
    external/tinyfiledialogs/tinyfiledialogs.cpp
)
target_include_directories(tinyfiledialogs PUBLIC external/tinyfiledialogs)


file(GLOB SOURCES "src/*.cpp")

add_executable(main ${SOURCES} )
target_compile_features(main PRIVATE cxx_std_23)
target_compile_definitions(main PRIVATE ASSETS_DIR="${PROJECT_SOURCE_DIR}/assets")
target_compile_definitions(main PRIVATE LEVEL_OUTPUT_DIR="${PROJECT_SOURCE_DIR}/LevelOutput")
target_link_libraries(main PRIVATE SFML::Graphics SFML::Audio djson::djson tinyfiledialogs)