cmake_minimum_required(VERSION 3.10)

project(egfx_test_projects)

set(EGFX_BASE_DIR "${CMAKE_CURRENT_LIST_DIR}/../../")

# Option to build all tests or let users select specific ones
option(BUILD_ALL_TESTS "Build all simulator tests" ON)

# Auto-discover all test_* directories
file(GLOB TEST_DIRS RELATIVE "${CMAKE_CURRENT_LIST_DIR}/prj" "${CMAKE_CURRENT_LIST_DIR}/prj/test_*")

foreach(TEST_DIR IN LISTS TEST_DIRS)
    # Create an option for each test
    option(BUILD_${TEST_DIR} "Build ${TEST_DIR}" ${BUILD_ALL_TESTS})

    if(BUILD_${TEST_DIR})
        message(STATUS "Including test: ${TEST_DIR}")
        set(TARGET ${TEST_DIR})
        add_subdirectory("${CMAKE_CURRENT_LIST_DIR}/prj/${TEST_DIR}" ${TEST_DIR})
    endif()
endforeach()