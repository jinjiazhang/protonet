cmake_minimum_required(VERSION 3.1)

add_definitions(-std=c++11)
add_executable(client client.cpp)
add_dependencies(client protonet)
target_link_libraries(client protonet)

add_executable(server server.cpp)
add_dependencies(server protonet)
target_link_libraries(server protonet)
include_directories(../protonet)
