cmake_minimum_required(VERSION 2.8)

project(instrument_functions)

SET(CMAKE_CXX_FLAGS "-Wall -g -std=c++11 -ldl")
SET(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -finstrument-functions -finstrument-functions-exclude-function-list=rfind,empty,end -finstrument-functions-exclude-file-list=/bits/stl,include/sys,usr/include,include/c++,hpp")

set(DEPS_INCLUDE_DIR "${PROJECT_SOUC_DIR} ")

include_directories(${DEPS_INCLUDE_DIR})

add_executable(example
  example.cc
  )
