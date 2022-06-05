cmake_minimum_required(VERSION 3.22)

project(chess)

set(CMAKE_CXX_STANDARD 23)

set(SFML_STATIC_LIBRARIES TRUE)

set(SFML_DIR ./SFML-2.5.1/lib/cmake/SFML)

find_package(SFML 2.5 COMPONENTS window graphics network REQUIRED)

add_executable(chess main.cpp ChessPieces.h Player.h Board.h Board.cpp Display.h Server.h Server.cpp Game.cpp Button.h Button.cpp StartMenu.cpp )

target_link_libraries(chess stdc++fs sfml-window sfml-graphics sfml-network)