cmake_minimum_required(VERSION 3.28)
project(volterra VERSION 0.1.0)

include(CTest)
include(FetchContent)   # Manages external dependencies during build

set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

string(APPEND CMAKE_CXX_FLAGS
      " -Wall -Wextra -Wpedantic -Wconversion -Wsign-conversion"
      " -Wshadow -Wimplicit-fallthrough -Wextra-semi -Wold-style-cast"
      " -fno-omit-frame-pointer")

if (CMAKE_CXX_COMPILER_ID MATCHES "GNU")
  string(APPEND CMAKE_CXX_FLAGS " -D_GLIBCXX_ASSERTIONS")
elseif (CMAKE_CXX_COMPILER_ID MATCHES "Clang")
  string(APPEND CMAKE_CXX_FLAGS " -D_LIBCPP_HARDENING_MODE=_LIBCPP_HARDENING_MODE_EXTENSIVE")
endif()

string(APPEND CMAKE_CXX_FLAGS_DEBUG " -fsanitize=address,undefined")
if (CMAKE_CXX_COMPILER_ID MATCHES "GNU")
  string(APPEND CMAKE_CXX_FLAGS " -D_GLIBCXX_SANITIZE_STD_ALLOCATOR")
endif()
string(APPEND CMAKE_EXE_LINKER_FLAGS_DEBUG " -fsanitize=address,undefined")

# Find SFML graphics component
find_package(SFML 2.6 COMPONENTS graphics)

if (NOT SFML_FOUND)
  message(STATUS "SFML not found, downloading via FetchContent")

  FetchContent_Declare(
    sfml
    GIT_REPOSITORY https://github.com/SFML/SFML.git
    GIT_TAG 2.6.0
  )
  FetchContent_MakeAvailable(sfml)

  add_library(sfml-graphics INTERFACE IMPORTED)
  target_link_libraries(sfml-graphics INTERFACE sfml-graphics)
endif()

add_executable(volterra main.cpp volterra.cpp graph_renderer.cpp utils.cpp)
target_link_libraries(volterra PRIVATE sfml-graphics)

if (BUILD_TESTING)
  add_executable(volterra.t volterra.test.cpp volterra.cpp)
  add_test(NAME volterra.t COMMAND volterra.t)
endif()

