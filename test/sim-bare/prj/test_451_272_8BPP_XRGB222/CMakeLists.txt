cmake_minimum_required(VERSION 3.10)

project(egfx_test_451_272_XRGB222)

message(STATUS "Cmake System name is ${CMAKE_SYSTEM_NAME}")

# Configure eGFX
if(NOT DEFINED EGFX_BASE_DIR)
    set(EGFX_BASE_DIR "${CMAKE_CURRENT_LIST_DIR}/../../../../")
    message(STATUS "Using egfx base of: ${EGFX_BASE_DIR}")
endif()

if(NOT DEFINED TARGET)
    set(TARGET ${PROJECT_NAME})
endif()

include("${EGFX_BASE_DIR}/test/sim-bare/sim-bare-boilerplate.cmake")

# Create simulator test with all boilerplate handled by the function
egfx_add_sim_test(
    ${TARGET}                           # Target name
    451                                 # Screen width
    272                                 # Screen height
    "8BPP_XRGB222"                      # Image depth
    "${CMAKE_CURRENT_LIST_DIR}/src"     # Test source folder
    "${CMAKE_CURRENT_LIST_DIR}/assets"  # Asset folder
    INITIAL_ZOOM 1
    SIM_GRID_ENABLE 0
    SIM_GRID_PIXEL_SIZE 2
    SIM_GRID_PIXEL_BORDER 1
    SIM_GRID_BACKGROUND_COLOR 0x101010
)