cmake_minimum_required(VERSION 3.9)
project(BoterKaasAI)

set(CMAKE_CXX_STANDARD 11)
set(CMAKE_CXX_FLAGS "-O3")

add_executable(BoterKaasAI main.cpp Utils.cpp Utils.h)