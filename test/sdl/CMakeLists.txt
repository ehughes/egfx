cmake_minimum_required(VERSION 3.10)

project(egfx_test_projects)

set(SDL2_DIR      "${CMAKE_CURRENT_LIST_DIR}/sdl2/cmake")
set(EGFX_LOC      "${CMAKE_CURRENT_LIST_DIR}/../../src")


set(TARGETS test_64_32_32BPP)

foreach(TARGET_NAME IN LISTS TARGETS) 
    set(TARGET ${TARGET_NAME})
    add_subdirectory("${CMAKE_CURRENT_LIST_DIR}/prj/${TARGET_NAME}/" ${TARGET_NAME})
endforeach()