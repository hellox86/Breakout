
project(Breakout)

cmake_minimum_required(VERSION 3.0.2 FATAL_ERROR)

if(WIN32)
  set(GUI_TYPE WIN32)
elseif(APPLE)
  set(GUI_TYPE MACOSX_BUNDLE)
endif()

set(SRC
    src/main.cpp
)

set(HDR)

set(RES)

add_definitions(-DWIN32 -D_WINDOWS -D_UNICODE -DUNICODE)

set(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} /SUBSYSTEM:WINDOWS,5.01")

add_custom_target(${PROJECT_NAME}_HEADERS SOURCES ${HDR})

add_executable(${CMAKE_PROJECT_NAME} ${GUI_TYPE} ${SRC} ${RES})
