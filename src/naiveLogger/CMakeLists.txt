cmake_minimum_required(VERSION 3.18)

project(naiveLogger)
set(CMAKE_CXX_STANDARD 17)
SET(CMAKE_BUILD_TYPE "Debug")
set(CMAKE_CPP_COMPILER iccarm)
include_directories(include/fmt)

set(SOURCE_FILES test.cpp)
find_package(fmt)
# add_subdirectory(fmt EXCLUDE_FROM_ALL)
add_executable(test ${SOURCE_FILES})
target_link_libraries(test fmt::fmt)
