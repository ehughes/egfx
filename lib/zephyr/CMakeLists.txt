# SPDX-License-Identifier: Apache-2.0

if(CONFIG_EGFX)

    zephyr_library()

    set(EGFX_BASE_DIR "${CMAKE_CURRENT_LIST_DIR}/../../")

    # Export EGFX_BASE_DIR for use by application CMakeLists.txt
    set(ZEPHYR_EGFX_MODULE_DIR "${EGFX_BASE_DIR}" CACHE INTERNAL "eGFX module directory")

    include(${EGFX_BASE_DIR}/cmake/egfx.cmake)

    # Get core eGFX source files
    egfx__get_core_src(EGFX_SRC_FILES)

    zephyr_library_sources(${EGFX_SRC_FILES})

    # Set all the undefined options to defaults
    egfx_config__set_defaults()

    # Configure the header file
    configure_file(
        "${EGFX_BASE_DIR}/cmake/egfx_config.h.in"
        "${APPLICATION_BINARY_DIR}/egfx/egfx_config.h"
        @ONLY
    )

    zephyr_include_directories(${APPLICATION_BINARY_DIR}/egfx)
    zephyr_include_directories(${EGFX_SRC_DIR})

endif()
