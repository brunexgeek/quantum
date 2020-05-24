project(quantum)

cmake_minimum_required(VERSION 2.8)

set(VERSION_MAJOR 0)
set(VERSION_MINOR 1)
set(VERSION_PATCH 0)

add_definitions(-DVERSION_MAJOR=${PROTOGEN_MAJOR})
add_definitions(-DVERSION_MINOR=${PROTOGEN_MINOR})
add_definitions(-DVERSION_PATCH=${PROTOGEN_PATCH})
add_definitions(-DVERSION_STRING="${VERSION_MAJOR}.${VERSION_MINOR}.${VERSION_PATCH}")

if (UNIX)
	set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wfatal-errors -pedantic -std=c++11 -Wl,--no-undefined -fPIC -Wall -Wextra -Wconversion -Wmaybe-uninitialized")
	set(CMAKE_CXX_FLAGS_RELEASE "-O3 -s")
endif()

if (CMAKE_BUILD_TYPE STREQUAL "Debug")
    add_definitions(-DBUILD_DEBUG)
else()
    set(CMAKE_BUILD_TYPE "Release")
    add_definitions(-DBUILD_RELEASE)
endif()
message("Build type: ${CMAKE_BUILD_TYPE}")

add_library(quantum_gui STATIC
    "source/api.cc"
    "source/widget.cc"
    "source/input.cc"
    "source/fonts.cc"
    "source/renderer.cc" )
target_include_directories(quantum_gui PUBLIC "include/")
target_link_libraries(quantum_gui "SDL2")
set_target_properties(quantum_gui PROPERTIES
    OUTPUT_NAME "quantum_gui"
    ARCHIVE_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}"
    LIBRARY_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}"
    RUNTIME_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}"
    PREFIX "lib" )

add_executable(demo "demo.cc")
target_link_libraries(demo quantum_gui)
set_target_properties(demo PROPERTIES
    OUTPUT_NAME "demo"
    ARCHIVE_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}"
    LIBRARY_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}"
    RUNTIME_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}" )

add_executable(export "fonts/export.cc")
set_target_properties(export PROPERTIES
    OUTPUT_NAME "export"
    ARCHIVE_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}"
    LIBRARY_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}"
    RUNTIME_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}" )