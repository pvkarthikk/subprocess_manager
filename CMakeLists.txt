cmake_minimum_required(VERSION 3.16)
# c/c++ standard
set(CMAKE_CXX_STANDARD 20)
# Set coverage flag
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -O0 -coverage")
# project name
project(subprocess_manager)
# library
add_library(subprocess_manager STATIC
    src/subprocess_manager.cpp
)
target_include_directories(subprocess_manager PRIVATE
    include
)
# unittest
add_executable(unittest 
    test/test_subprocess_manager.cpp
)
target_include_directories(unittest PRIVATE 
    include
)
target_link_libraries(unittest subprocess_manager
)
add_executable(task
    test/task.cpp
)
include(CTest)
add_test(NAME Subprocess COMMAND unittest)


