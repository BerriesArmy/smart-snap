cmake_minimum_required(VERSION 3.21)
project(SmartSnap VERSION 1.0.0)

# Pull in Geode. If you set up this project with `geode new`, this section
# is normally already wired up for you via the geode-cli template; this is
# the manual equivalent.
if (NOT DEFINED ENV{GEODE_SDK})
    message(FATAL_ERROR "Please define GEODE_SDK environment variable to point to the Geode SDK (run `geode config`)")
else()
    message(STATUS "Found Geode: $ENV{GEODE_SDK}")
    set(GEODE_SDK $ENV{GEODE_SDK})
endif()

add_subdirectory($ENV{GEODE_SDK} ${CMAKE_CURRENT_BINARY_DIR}/geode)

file(GLOB SOURCES
    src/*.cpp
)

add_library(${PROJECT_NAME} SHARED ${SOURCES})

target_include_directories(${PROJECT_NAME} PRIVATE src)

setup_geode_mod(${PROJECT_NAME})
