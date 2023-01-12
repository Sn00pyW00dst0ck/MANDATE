cmake_minimum_required(VERSION 3.0)
project(MANDATE VERSION 0.0.1)

include(CTest)
enable_testing()

# Get the sources
file (GLOB_RECURSE Foo_SOURCES CONFIGURE_DEPENDS "src/*.cpp")
file (GLOB_RECURSE Foo_HEADERS CONFIGURE_DEPENDS "src/*.h")

# Set the include directories
include_directories(src/)

# Add executable
add_executable(mandate ${Foo_SOURCES})

# Set CPACK
set(CPACK_PROJECT_NAME ${PROJECT_NAME})
set(CPACK_PROJECT_VERSION ${PROJECT_VERSION})
include(CPack)