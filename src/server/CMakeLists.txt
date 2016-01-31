cmake_minimum_required(VERSION 3.3)
project(Quizzer)

set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -Wall -ldl -Werror -lpthread -lsqlite3")

set(SERVER_SOURCE_FILES server.c)
add_executable(server ${SERVER_SOURCE_FILES})

set(INIT_SOURCE_FILES init_tables.c)
add_executable(init ${INIT_SOURCE_FILES})

