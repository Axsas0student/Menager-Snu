cmake_minimum_required(VERSION 3.5)

project(MSnu_dll VERSION 1.0 LANGUAGES CXX)

add_library(SleepAnalyzerLibrary SHARED
    SleepAnalyzer.cpp
    SleepAnalyzer.h
    API.h
)

target_include_directories(SleepAnalyzerLibrary PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})
