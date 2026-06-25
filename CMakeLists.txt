cmake_minimum_required(VERSION 3.25)
project(gitpp)

set(CMAKE_CXX_STANDARD 23)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

find_package(OpenSSL REQUIRED)
find_package(ZLIB REQUIRED)

include_directories(include)
add_executable(gitpp
        src/main.cpp
        src/repository.cpp
        src/object.cpp
        src/tree.cpp
)
target_link_libraries(gitpp PRIVATE OpenSSL::Crypto ZLIB::ZLIB)

enable_testing()
add_subdirectory(tests)