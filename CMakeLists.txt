cmake_minimum_required(VERSION 3.25)
project(gitpp)

set(CMAKE_CXX_STANDARD 23)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

include_directories(include)
add_executable(gitpp
        src/main.cpp
        src/repository.cpp
)