cmake_minimum_required(VERSION 3.1)

include(CheckCXXCompilerFlag)
check_cxx_compiler_flag("-std=c++11" SUPPORT_CXX11)
if (NOT SUPPORT_CXX11)
    message(FATAL_ERROR "compiler not support c++11")
endif()

if ("${CMAKE_CXX_COMPILER_ID}" STREQUAL "GNU")
    set(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} -Wl,-E")
endif()

add_definitions(-std=c++11)
add_executable(client client.cpp)
add_dependencies(client protonet)
target_link_libraries(client protonet)

add_executable(server server.cpp)
add_dependencies(server protonet)
target_link_libraries(server protonet)
include_directories(../protonet)
