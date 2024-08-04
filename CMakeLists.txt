cmake_minimum_required(VERSION 3.17)
# c/c++ standard
set(CMAKE_CXX_STANDARD 20)
# project name
project(subprocess_manager)
# library
add_library(subprocess_manager STATIC
    src/subprocess_manager.cpp
)
target_include_directories(subprocess_manager PRIVATE
    include
)
# unittest and resources
add_executable(task 
    test/task.cpp
)
add_executable(unittest 
    test/test_subprocess_manager.cpp
)
target_include_directories(unittest PRIVATE 
    include
)
target_link_libraries(unittest subprocess_manager
)


