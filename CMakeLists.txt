cmake_minimum_required(VERSION 3.8)
project(set01c03)

set(CMAKE_CXX_STANDARD 11)

set(SOURCE_FILES main.cpp)

add_executable(set01c03 ${SOURCE_FILES})
target_link_libraries(set01c03 PocoFoundation)

add_executable(s01c04 s01c04.cpp)
target_link_libraries(s01c04 PocoFoundation)

add_executable(s01c05 s01c05.cpp)

add_executable(s01c06 s01c06.cpp)
target_link_libraries(s01c06 PocoFoundation)

link_directories(/usr/local/opt/openssl/lib)
include_directories(/usr/local/opt/openssl/include)
add_executable(s01c07 s01c07.cpp)
target_link_libraries(s01c07 PocoFoundation crypto)

add_executable(s01c08 s01c08.cpp)
target_link_libraries(s01c08 PocoFoundation)

add_executable(s02c01 s02c01.cpp)

add_executable(segfault segfault.cpp)