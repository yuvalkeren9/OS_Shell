cmake_minimum_required(VERSION 3.19)
project(smash)

set(CMAKE_CXX_STANDARD 14)

add_executable(skeleton_smash smash.cpp Commands.cpp signals.cpp BuiltInCommands.cpp ExternalCommands.cpp JobsList.cpp)