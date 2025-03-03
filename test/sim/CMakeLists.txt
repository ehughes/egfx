cmake_minimum_required(VERSION 3.10)

project(egfx_test_projects)

set(EGFX_LOC "${CMAKE_CURRENT_LIST_DIR}/../../")

set(TARGETS test_64_32_32BPP test_451_272_8BPP_XRGB222 test_256_64_4BPP)

# Process each target
foreach(TARGET_NAME IN LISTS TARGETS) 
    set(TARGET ${TARGET_NAME})
    add_subdirectory("${CMAKE_CURRENT_LIST_DIR}/prj/${TARGET_NAME}/" ${TARGET_NAME})
endforeach()