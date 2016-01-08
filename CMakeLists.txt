cmake_minimum_required(VERSION 3.3)
project(dino)
set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -Wall -Werror")

set(SERVER_SOURCE_FILES server.c)
set(INIT_SOURCE_FILES init_tables.c)
add_executable(server ${SERVER_SOURCE_FILES})
add_executable(init ${INIT_SOURCE_FILES})
