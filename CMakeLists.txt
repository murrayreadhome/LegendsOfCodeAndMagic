﻿# CMakeList.txt : CMake project for LegendsOfCodeAndMagic, include source and define
# project specific logic here.
#
cmake_minimum_required (VERSION 3.8)

project(LegendsOfCodeAndMagic)

add_compile_definitions(DEVELOPMENT)

# Add source to this project's executable.
add_executable (LegendsOfCodeAndMagic "LegendsOfCodeAndMagic.cpp" "LegendsOfCodeAndMagic.h" "CardInfo.cpp" "CardInfo.h" "GameController.cpp" "GameController.h" "OldPlayers.h")

# TODO: Add tests and install targets if needed.
