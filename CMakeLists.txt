cmake_minimum_required(VERSION 2.6)

project(rtflames)

add_subdirectory(engine)
add_subdirectory(src)

#if(not CMAKE_BUILD_TYPE strequal Debug and not CMAKE_BUILD_TYPE strequal Profiling)
#  set(CMAKE_BUILD_TYPE Release)
#endif(not CMAKE_BUILD_TYPE strequal Debug and not CMAKE_BUILD_TYPE strequal Profiling)
