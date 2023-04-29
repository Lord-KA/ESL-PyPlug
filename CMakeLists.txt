cmake_minimum_required(VERSION 3.1)

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++20" CACHE STRING "Comment" FORCE)
set(CMAKE_CXX_FLAGS_RELEASE "${CMAKE_CXX_FLAGS} -D NDEBUG" CACHE STRING "Comment" FORCE)
set(CMAKE_CXX_FLAGS_SANITIZER "${CMAKE_CXX_FLAGS} -Wall -Wextra -Wformat=2 -fsanitize=address,undefined -g" CACHE STRING "Comment" FORCE)

# Enable debug symbols by default
# must be done before project() statement
if(NOT CMAKE_BUILD_TYPE)
  set(CMAKE_BUILD_TYPE Debug CACHE STRING "Choose the type of build (Debug or Release)" FORCE)
endif()
# (you can also set it on the command line: -D CMAKE_BUILD_TYPE=Release)

include(FetchContent)

FetchContent_Declare(
  utils
  GIT_REPOSITORY https://github.com/Lord-KA/CPPUtils.git
  GIT_TAG        release-0.1
)

FetchContent_GetProperties(utils)
if(NOT utils_POPULATED)
  FetchContent_Populate(utils)
  include_directories(${utils_SOURCE_DIR})
endif()

FetchContent_Declare(
  commonTools
  GIT_REPOSITORY https://github.com/mishaglik/Elpidifor-s-legacy.git
  GIT_TAG        v2.0.1
)

FetchContent_GetProperties(commonTools)
if(NOT commonTools_POPULATED)
  FetchContent_Populate(commonTools)
  include_directories(./build/_deps/commontools-src)
endif()

# Define sources and executable
add_executable(standalone-pyplug ./standalone-pyplug.cpp)

FetchContent_Declare(
  pybind11
  GIT_REPOSITORY https://github.com/pybind/pybind11.git
  GIT_TAG        v2.10.4
)

FetchContent_GetProperties(pybind11)
if(NOT pybind11_POPULATED)
  FetchContent_Populate(pybind11)
  add_subdirectory(./build/_deps/pybind11-src)
endif()

target_link_libraries(standalone-pyplug pybind11::embed)

add_library(pyplug SHARED pyplug.cpp)
target_link_libraries(pyplug pybind11::embed)
