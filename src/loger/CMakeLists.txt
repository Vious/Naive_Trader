include_directories(${CMAKE_INCLUDE_PATH})

link_directories(${CMAKE_LIBRARY_PATH})

add_library(local_loger SHARED "log_wrapper.cpp" "nanolog.cpp")