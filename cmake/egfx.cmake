if(NOT DEFINED EGFX_BASE_DIR)
    message(FATAL_ERROR "EGFX_BASE_DIR is not defined. Please set EGFX_BASE_DIR before including this file.")
endif()



set(EGFX_SRC_DIR "${EGFX_BASE_DIR}/src/")
set(EGFX_TOOLS_DIR "${EGFX_BASE_DIR}/tools")
set(EGFX_IO_DIR "${EGFX_BASE_DIR}/io")

macro(set_default var_name default_value)
    if(NOT DEFINED ${var_name})
        set(${var_name} ${default_value})
    endif()
endmacro()

macro(egfx_config__set_defaults)
    set_default(CONFIG_EGFX_MAX_STRING_LEN 256)
endmacro()

function(egfx__get_core_src OUTPUT_VAR)

    set(EGFX_CORE_SOURCES
        "${EGFX_SRC_DIR}/egfx_animator.c"
        "${EGFX_SRC_DIR}/egfx_blit.c"
        "${EGFX_SRC_DIR}/egfx_bmp.c"
        "${EGFX_SRC_DIR}/egfx_pixel_ops.c"
        "${EGFX_SRC_DIR}/egfx_plane.c"
        "${EGFX_SRC_DIR}/egfx_primitive_ops.c"
        "${EGFX_SRC_DIR}/egfx_text.c"

        "${EGFX_SRC_DIR}/font_3_5.c"
        "${EGFX_SRC_DIR}/font_5_7.c"
        "${EGFX_SRC_DIR}/font_10_14.c"
      )

    set(${OUTPUT_VAR} ${EGFX_CORE_SOURCES} PARENT_SCOPE)

endfunction()

function(egfx__add_to_target TARGET_NAME)

    # Set defaults
    egfx_config__set_defaults()
    
    # Set up directories

    # Configure the header file
    configure_file(
        "${EGFX_BASE_DIR}/cmake/egfx_config.h.in"
        "${CMAKE_CURRENT_BINARY_DIR}/egfx/egfx_config.h"
        @ONLY
    )
    
    # Get the core sources
    egfx__get_core_src(EGFX_SOURCES)
    
    # Add sources to target
    target_sources(${TARGET_NAME} PRIVATE ${EGFX_SOURCES})
    
    # Add include directories
    target_include_directories(${TARGET_NAME} PRIVATE 
        "${CMAKE_CURRENT_BINARY_DIR}/egfx"
        "${EGFX_SRC_DIR}"
    )
endfunction()

#generate our local assets

function(generate_sprites IMAGE_PLANE_TYPE INPUT_FOLDER OUTPUT_FOLDER OUTPUT_NAME)
    find_package(Python3 REQUIRED COMPONENTS Interpreter)

    # Auto-detect EGFX_BASE_DIR if not set (for Zephyr module usage)
    if(NOT DEFINED EGFX_BASE_DIR)
        # Try to find egfx module in Zephyr workspace
        if(DEFINED ZEPHYR_EGFX_MODULE_DIR)
            set(EGFX_BASE_DIR "${ZEPHYR_EGFX_MODULE_DIR}")
        elseif(EXISTS "${CMAKE_CURRENT_LIST_DIR}/../tools/egfx_sprite_tool.py")
            # We're being called from egfx.cmake itself
            get_filename_component(EGFX_BASE_DIR "${CMAKE_CURRENT_LIST_DIR}/.." ABSOLUTE)
        else()
            message(FATAL_ERROR "EGFX_BASE_DIR is not defined and could not be auto-detected. Please set EGFX_BASE_DIR before calling generate_sprites()")
        endif()
    endif()

    # Set tool directory
    set(LOCAL_EGFX_TOOLS_DIR "${EGFX_BASE_DIR}/tools")

    message(STATUS "Generating sprite files during configuration... ")
    message(STATUS "  Using EGFX tools from: ${LOCAL_EGFX_TOOLS_DIR}")

    if(OUTPUT_NAME)
        execute_process(
            COMMAND ${Python3_EXECUTABLE} "${LOCAL_EGFX_TOOLS_DIR}/egfx_sprite_tool.py" "${INPUT_FOLDER}" "eGFX_IMAGE_PLANE_${IMAGE_PLANE_TYPE}" "${OUTPUT_FOLDER}" "--output-name" "${OUTPUT_NAME}"
            RESULT_VARIABLE RESULT_CODE
            OUTPUT_VARIABLE TOOL_OUTPUT
            ERROR_VARIABLE TOOL_ERROR
        )
    else()
        execute_process(
            COMMAND ${Python3_EXECUTABLE} "${LOCAL_EGFX_TOOLS_DIR}/egfx_sprite_tool.py" "${INPUT_FOLDER}" "eGFX_IMAGE_PLANE_${IMAGE_PLANE_TYPE}" "${OUTPUT_FOLDER}"
            RESULT_VARIABLE RESULT_CODE
            OUTPUT_VARIABLE TOOL_OUTPUT
            ERROR_VARIABLE TOOL_ERROR
        )
    endif()

    if(NOT RESULT_CODE EQUAL 0)
        message(FATAL_ERROR "Sprite generation failed: ${TOOL_ERROR}")
    else()
        message(STATUS "Successfully generated sprite files")
    endif()
endfunction()

# Function to generate eGFX fonts from TrueType files
# Usage:
#   egfx_generate_font_from_ttf(
#     TTF_FILE "path/to/font.ttf"
#     SIZES 10 12 14 16
#     BPP 1bpp 4bpp
#     OUTPUT_DIR "${CMAKE_CURRENT_LIST_DIR}/assets"
#     GENERATE_CODE
#   )
function(egfx_generate_font_from_ttf)
    set(options GENERATE_CODE)
    set(oneValueArgs TTF_FILE OUTPUT_DIR)
    set(multiValueArgs SIZES BPP)
    cmake_parse_arguments(FONT "${options}" "${oneValueArgs}" "${multiValueArgs}" ${ARGN})

    # Find Python
    find_package(Python3 COMPONENTS Interpreter REQUIRED)

    # Auto-detect EGFX_BASE_DIR if not set (for Zephyr module usage)
    if(NOT DEFINED EGFX_BASE_DIR)
        # Try to find egfx module in Zephyr workspace
        if(DEFINED ZEPHYR_EGFX_MODULE_DIR)
            set(EGFX_BASE_DIR "${ZEPHYR_EGFX_MODULE_DIR}")
        elseif(EXISTS "${CMAKE_CURRENT_LIST_DIR}/../tools/egfx_ttf_to_font.py")
            # We're being called from egfx.cmake itself
            get_filename_component(EGFX_BASE_DIR "${CMAKE_CURRENT_LIST_DIR}/.." ABSOLUTE)
        else()
            message(FATAL_ERROR "EGFX_BASE_DIR is not defined and could not be auto-detected. Please set EGFX_BASE_DIR before calling egfx_generate_font_from_ttf()")
        endif()
    endif()

    # Set tool directory
    set(LOCAL_EGFX_TOOLS_DIR "${EGFX_BASE_DIR}/tools")

    # Validate required arguments
    if(NOT FONT_TTF_FILE)
        message(FATAL_ERROR "egfx_generate_font_from_ttf: TTF_FILE is required")
    endif()

    if(NOT FONT_SIZES)
        message(FATAL_ERROR "egfx_generate_font_from_ttf: SIZES is required")
    endif()

    # Set defaults
    if(NOT FONT_BPP)
        set(FONT_BPP "1bpp")
    endif()

    if(NOT FONT_OUTPUT_DIR)
        set(FONT_OUTPUT_DIR "${CMAKE_CURRENT_LIST_DIR}/generated_fonts")
    endif()

    # Build the command
    set(TOOL_PATH "${LOCAL_EGFX_TOOLS_DIR}/egfx_ttf_to_font.py")
    set(FONT_CMD ${Python3_EXECUTABLE} ${TOOL_PATH} ${FONT_TTF_FILE})

    # Add size arguments
    list(APPEND FONT_CMD --size)
    foreach(SIZE ${FONT_SIZES})
        list(APPEND FONT_CMD ${SIZE})
    endforeach()

    # Add BPP arguments
    list(APPEND FONT_CMD --bpp)
    foreach(BPP_OPTION ${FONT_BPP})
        list(APPEND FONT_CMD ${BPP_OPTION})
    endforeach()

    # Add output directory
    list(APPEND FONT_CMD --output ${FONT_OUTPUT_DIR})

    # Add generate flag if requested
    if(FONT_GENERATE_CODE)
        list(APPEND FONT_CMD --generate)
    endif()

    # Execute the command
    message(STATUS "Generating font from ${FONT_TTF_FILE}")
    execute_process(
        COMMAND ${FONT_CMD}
        WORKING_DIRECTORY ${LOCAL_EGFX_TOOLS_DIR}
        RESULT_VARIABLE RESULT
        OUTPUT_VARIABLE OUTPUT
        ERROR_VARIABLE ERROR
    )

    if(NOT RESULT EQUAL 0)
        message(FATAL_ERROR "Font generation failed: ${ERROR}")
    else()
        message(STATUS "${OUTPUT}")
    endif()
endfunction()