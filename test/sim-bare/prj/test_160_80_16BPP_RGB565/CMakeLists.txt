cmake_minimum_required(VERSION 3.10)

project(egfx_test_160_80_16BPP_RGB565)

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
# Display: ST7735R 160x80 (charger tester display, landscape orientation)
egfx_add_sim_test(
    ${TARGET}                           # Target name
    160                                 # Screen width
    80                                  # Screen height
    "16BPP_RGB565"                      # Image depth
    "${CMAKE_CURRENT_LIST_DIR}/src"     # Test source folder
    "${CMAKE_CURRENT_LIST_DIR}/assets"  # Asset folder
    INITIAL_ZOOM 4                      # 4x zoom for small display
    SIM_GRID_ENABLE 0
)
