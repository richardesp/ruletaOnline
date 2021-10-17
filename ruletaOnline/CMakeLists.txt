cmake_minimum_required(VERSION 3.0)

project(ruleta_online)

add_executable(server.exe ./src/server/server.c)
add_executable(client.exe ./src/client/client.c)

set(CMAKE_CXX_FLAGS "-Wall")  