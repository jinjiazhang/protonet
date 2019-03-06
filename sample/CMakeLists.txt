cmake_minimum_required(VERSION 3.1)

add_executable(client client.cpp)
add_executable(server server.cpp)
include_directories(../protonet)
