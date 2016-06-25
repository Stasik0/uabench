cmake_minimum_required(VERSION 2.8.11)
project(uabench CXX)
include(ExternalProject)
set (
  CMAKE_CXX_FLAGS
  "${CMAKE_CXX_FLAGS} -pthread"
)

# Set default build type.
if(NOT CMAKE_BUILD_TYPE)
    message(STATUS "CMAKE_BUILD_TYPE not given; setting to 'Release'.")
    set(CMAKE_BUILD_TYPE "Release" CACHE STRING "Choose the type of build." FORCE)
endif()

ExternalProject_Add(open62541
DOWNLOAD_COMMAND ""
CMAKE_ARGS -DCMAKE_INSTALL_PREFIX=${CMAKE_SOURCE_DIR}/external
           -DCMAKE_BUILD_TYPE=${CMAKE_BUILD_TYPE}
           -DUA_ENABLE_AMALGAMATION=ON
SOURCE_DIR ${CMAKE_SOURCE_DIR}/external/open62541
BINARY_DIR ${CMAKE_SOURCE_DIR}/external_generated
INSTALL_COMMAND ""
)

include_directories(${PROJECT_SOURCE_DIR}/include)
include_directories(${PROJECT_SOURCE_DIR}/external_generated)
link_directories(${PROJECT_SOURCE_DIR}/external_generated)

add_executable(uabench src/uabench.cpp src/Client.cpp src/InputParser.cpp)
target_link_libraries(uabench libopen62541-static.a)

target_compile_features(uabench PRIVATE cxx_range_for)
add_dependencies(uabench open62541)
