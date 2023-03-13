cmake_minimum_required(VERSION 2.8)

project(instrument_functions)

SET (CMAKE_CXX_FLAGS "-Wall -g -std=c++11 ")
set(DEPS_INCLUDE_DIR "${PROJECT_SOUC_DIR} ")

include_directories(${DEPS_INCLUDE_DIR})

set(DEP_LIBS
  stdc++
  )

add_executable(decoder
  example.cc
  )
