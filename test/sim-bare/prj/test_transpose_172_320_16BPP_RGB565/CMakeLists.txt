cmake_minimum_required(VERSION 3.10)

project(egfx_test_transpose_172_320_16BPP_RGB565)

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

# Create simulator test with PHYSICAL dimensions (172x320)
# The display window will show 172 wide x 320 tall
egfx_add_sim_test(
    ${TARGET}                           # Target name
    172                                 # Physical screen width
    320                                 # Physical screen height
    "16BPP_RGB565"                      # Image depth
    "${CMAKE_CURRENT_LIST_DIR}/src"     # Test source folder
    "${CMAKE_CURRENT_LIST_DIR}/assets"  # Asset folder
    INITIAL_ZOOM 2
    SIM_GRID_ENABLE 0
)

# Generate fonts from TrueType files during build

# Fragment Mono 14px 1BPP
egfx_generate_font_from_ttf(
  TTF_FILE "${CMAKE_CURRENT_LIST_DIR}/assets/fragment_mono.ttf"
  SIZES 14
  BPP 1bpp
  OUTPUT_DIR "${CMAKE_CURRENT_LIST_DIR}/assets"
  GENERATE_CODE
)

# Fragment Mono 14px 8BPP
egfx_generate_font_from_ttf(
  TTF_FILE "${CMAKE_CURRENT_LIST_DIR}/assets/fragment_mono.ttf"
  SIZES 14
  BPP 8bpp
  OUTPUT_DIR "${CMAKE_CURRENT_LIST_DIR}/assets"
  GENERATE_CODE
)

# Fragment Mono 28px 1BPP
egfx_generate_font_from_ttf(
  TTF_FILE "${CMAKE_CURRENT_LIST_DIR}/assets/fragment_mono.ttf"
  SIZES 28
  BPP 1bpp
  OUTPUT_DIR "${CMAKE_CURRENT_LIST_DIR}/assets"
  GENERATE_CODE
)

# Fragment Mono 28px 8BPP
egfx_generate_font_from_ttf(
  TTF_FILE "${CMAKE_CURRENT_LIST_DIR}/assets/fragment_mono.ttf"
  SIZES 28
  BPP 8bpp
  OUTPUT_DIR "${CMAKE_CURRENT_LIST_DIR}/assets"
  GENERATE_CODE
)

# Orbitron 16px 8BPP
egfx_generate_font_from_ttf(
  TTF_FILE "${CMAKE_CURRENT_LIST_DIR}/assets/orbitron.ttf"
  SIZES 16
  BPP 8bpp
  OUTPUT_DIR "${CMAKE_CURRENT_LIST_DIR}/assets"
  GENERATE_CODE
)

# Add generated fonts to build
target_sources(${TARGET} PRIVATE
  "${CMAKE_CURRENT_LIST_DIR}/assets/font_fragment_mono_14px_1bpp/font_fragment_mono_14px_1bpp.c"
  "${CMAKE_CURRENT_LIST_DIR}/assets/font_fragment_mono_14px_8bpp/font_fragment_mono_14px_8bpp.c"
  "${CMAKE_CURRENT_LIST_DIR}/assets/font_fragment_mono_28px_1bpp/font_fragment_mono_28px_1bpp.c"
  "${CMAKE_CURRENT_LIST_DIR}/assets/font_fragment_mono_28px_8bpp/font_fragment_mono_28px_8bpp.c"
  "${CMAKE_CURRENT_LIST_DIR}/assets/font_orbitron_16px_8bpp/font_orbitron_16px_8bpp.c"
)

target_include_directories(${TARGET} PRIVATE
  "${CMAKE_CURRENT_LIST_DIR}/assets/font_fragment_mono_14px_1bpp"
  "${CMAKE_CURRENT_LIST_DIR}/assets/font_fragment_mono_14px_8bpp"
  "${CMAKE_CURRENT_LIST_DIR}/assets/font_fragment_mono_28px_1bpp"
  "${CMAKE_CURRENT_LIST_DIR}/assets/font_fragment_mono_28px_8bpp"
  "${CMAKE_CURRENT_LIST_DIR}/assets/font_orbitron_16px_8bpp"
)

# See egfx/src/FONT_GENERATION_CMAKE.md for full documentation
