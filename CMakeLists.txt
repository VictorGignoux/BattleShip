cmake_minimum_required(VERSION 3.29)
project(tp4_5_battleship C)

set(CMAKE_C_STANDARD 90)

add_executable(client client/client.c
        typeDef.h
        client/client.h)
add_executable(server server/server.c
        typeDef.h
        server/server.h)