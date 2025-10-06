cmake_minimum_required(VERSION 3.20.0)

# Configure eGFX
set(EGFX_BASE_DIR "${CMAKE_CURRENT_LIST_DIR}/../../../../")

find_package(Zephyr REQUIRED HINTS $ENV{ZEPHYR_BASE})
project(egfx_test_320_172_16BPP_RGB565)

# Add application source
target_sources(app PRIVATE src/main.c)

# Add application includes (for eGFX_Config.h)
target_include_directories(app PRIVATE src)

# eGFX library is included via the Zephyr module system
# See egfx/lib/zephyr/CMakeLists.txt
