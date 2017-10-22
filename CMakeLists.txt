cmake_minimum_required(VERSION 3.8)
project(set01c03)

if(NOT CMAKE_BUILD_TYPE)
    set(CMAKE_BUILD_TYPE Debug)
endif()

set(CMAKE_CXX_FLAGS "-Wall -Wextra")
#set(CMAKE_CXX_FLAGS_DEBUG "-g")
set(CMAKE_CXX_FLAGS_RELEASE "-O3")

set(CMAKE_CXX_STANDARD 14)

set(SOURCE_FILES utils.h AESBlockCipher.h)
#list(APPEND LDFLAGS -Wl,-no_pie)
set(CMAKE_EXE_LINKER_FLAGS -Wl,-no_pie)

#add_executable(set01c03 ${SOURCE_FILES})
#target_link_libraries(set01c03 PocoFoundation)

add_executable(s01c04 s01c04.cpp)
target_link_libraries(s01c04 PocoFoundation)

add_executable(s01c05 s01c05.cpp)

add_executable(s01c06 s01c06.cpp)
target_link_libraries(s01c06 PocoFoundation)

link_directories(/usr/local/opt/openssl/lib)
include_directories(/usr/local/opt/openssl/include)
add_executable(s01c07 ${SOURCE_FILES} s01c07.cpp)
target_link_libraries(s01c07 PocoFoundation crypto)

add_executable(s01c08 s01c08.cpp)
target_link_libraries(s01c08 PocoFoundation)

add_executable(s02c01 s02c01.cpp)

add_executable(s02c02 ${SOURCE_FILES} s02c02.cpp)
target_link_libraries(s02c02 PocoFoundation crypto)

add_executable(s02c11 ${SOURCE_FILES} s02c11.cpp)
target_link_libraries(s02c11 PocoFoundation crypto tcmalloc)

add_executable(s02c12 ${SOURCE_FILES} s02c12.cpp)
target_link_libraries(s02c12 PocoFoundation crypto tcmalloc)

add_executable(s02c13 ${SOURCE_FILES} s02c13.cpp)
target_link_libraries(s02c13 PocoFoundation crypto tcmalloc)

add_executable(s02c14 ${SOURCE_FILES} s02c14.cpp)
# target_link_libraries(s02c14 PocoFoundation crypto tcmalloc profiler)
target_link_libraries(s02c14 PocoFoundation crypto)

add_executable(segfault segfault.cpp)
add_executable(sandbox sandbox.cpp)