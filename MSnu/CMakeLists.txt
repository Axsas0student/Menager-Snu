cmake_minimum_required(VERSION 3.5)

project(MSnu VERSION 1.0 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 11)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

add_executable(MSnu main.cpp)

target_link_libraries(MSnu PRIVATE SleepAnalyzerLibrary)

target_include_directories(MSnu PRIVATE ${CMAKE_CURRENT_SOURCE_DIR} ${CMAKE_SOURCE_DIR}/MSnu_dll)
