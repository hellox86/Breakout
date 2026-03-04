
project(Breakout)

cmake_minimum_required(VERSION 3.6 FATAL_ERROR)

if(WIN32)
  set(GUI_TYPE WIN32)
endif()

file(GLOB SRC "src/main.cpp")
file(GLOB HDR "header/resource.h")
file(GLOB RES "res/*.rc")

add_definitions(-DWIN32 -D_WINDOWS -D_UNICODE -DUNICODE)

set(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} /SUBSYSTEM:WINDOWS,5.01")

add_executable(${CMAKE_PROJECT_NAME} ${GUI_TYPE} ${SRC} ${HDR} ${RES})
