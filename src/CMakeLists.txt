cmake_minimum_required(3.16)

project("naiveTrader")

set(CMAKE_CXX_STANDARD 17)

## Currently, I will assume it only runs in linux system
add_definitions("-fPIC")
set(SYS_LIBS pthread)

# older version of executable file var
# set(EXECUTABLE_OUTPUT_PATH ${PROJECT_SOURCE_DIR}/../bin)
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${PROJECT_SOURCE_DIR}/../bin)
set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${PROJECT_SOURCE_DIR}/../bin)

set(CMAKE_DEBUG_POSTFIX "_d")
# set(CMAKE_CXX_FLAGS_RELEASE "-O2")
set(CMAKE_INCLUDE_PATH ${CMAKE_LIBRARY_PATH}
    "${PROJECT_SOURCE_DIR}/include"
    "${PROJECT_SOURCE_DIR}/utils"
)

set(CMAKE_LIBRARY_PATH ${CMAKE_LIBRARY_PATH} "${PROJECT_SOURCE_DIR}/../api/CTP_V6.6.9_20220920")

add_subdirectory("apiadapter")

add_subdirectory("loger")

add_subdirectory("example")




