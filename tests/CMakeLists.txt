cmake_minimum_required(VERSION 3.25)
project(gitpp_tests)

set(CMAKE_CXX_STANDARD 23)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

find_package(OpenSSL REQUIRED)
find_package(ZLIB REQUIRED)

# test_init
add_executable(test_init test_init.cpp ../src/repository.cpp)
target_include_directories(test_init PRIVATE ../src)

# test_hash_object
add_executable(test_hash_object test_hash_object.cpp ../src/object.cpp ../src/repository.cpp)
target_include_directories(test_hash_object PRIVATE ../src)
target_link_libraries(test_hash_object PRIVATE OpenSSL::Crypto ZLIB::ZLIB)

# test_find_object
add_executable(test_find_object test_find_object.cpp ../src/object.cpp ../src/repository.cpp)
target_include_directories(test_find_object PRIVATE ../src)
target_link_libraries(test_find_object PRIVATE OpenSSL::Crypto ZLIB::ZLIB)