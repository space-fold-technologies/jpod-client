cmake_minimum_required(VERSION 3.16...3.23)

project(client)
# Not ideal to use this global variable, but necessary to make sure
# that tooling and projects use the same version
set(CMAKE_CXX_STANDARD 17)

# strongly encouraged to enable this globally to avoid conflicts between
# -Wpedantic being enabled and -std=c++20 and -std=gnu++20 for example
# when compiling with PCH enabled
set(CMAKE_CXX_EXTENSIONS OFF)
set(CMAKE_MODULE_PATH "${CMAKE_CURRENT_LIST_DIR}/cmake")
include(utilities)
include(rc)
include(conan)
run_conan()

set(CMAKE_INCLUDE_CURRENT_DIR ON)

option(BUILD_TESTS "Build Unit tests" ON)

include_directories(include)
add_subdirectory(src)