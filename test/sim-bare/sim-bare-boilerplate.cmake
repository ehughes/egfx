
if(NOT DEFINED EGFX_BASE_DIR)
    message(FATAL_ERROR "EGFX_BASE_DIR is not defined. Please set EGFX_BASE_DIR before including this file.")
endif()

include("${EGFX_BASE_DIR}/cmake/egfx.cmake")



set(GLFW_DIR "${EGFX_BASE_DIR}/test/sim-bare/glfw-3.4.bin.WIN64")
set(IMGUI_DIR "${EGFX_BASE_DIR}/test/sim-bare/imgui")

if(${CMAKE_SYSTEM_NAME} STREQUAL "Windows")
    message(STATUS "Configuring SDL for Windows")
    set(SDL2_DIR "${EGFX_BASE_DIR}/test/sim-bare/sdl2/cmake")
else()
    message(STATUS "Configuring SDL for Linux")
endif()

find_package(SDL2 REQUIRED)
include_directories(${SDL2_INCLUDE_DIRS})

# Find OpenGL
find_package(OpenGL REQUIRED)

# Configure GLFW
message(STATUS "Using local GLFW from: ${GLFW_DIR}")

set(GLFW_INCLUDE_DIR "${GLFW_DIR}/include")
if(CMAKE_SIZEOF_VOID_P EQUAL 8)
    # 64-bit
    set(GLFW_LIBRARY "${GLFW_DIR}/lib-vc2022/glfw3.lib")
else()
    # 32-bit
    set(GLFW_LIBRARY "${GLFW_DIR}/lib-vc2022/glfw3.lib")
endif()


# Check if GLFW files exist
if(NOT EXISTS "${GLFW_INCLUDE_DIR}/GLFW/glfw3.h")
    message(FATAL_ERROR "GLFW header not found at ${GLFW_INCLUDE_DIR}/GLFW/glfw3.h")
endif()

if(NOT EXISTS "${GLFW_LIBRARY}")
    message(FATAL_ERROR "GLFW library not found at ${GLFW_LIBRARY}")
    # Try to find the library in alternative lib directories if the default one doesn't work
    foreach(LIB_DIR "lib-vc2022" "lib-vc2019" "lib-vc2017" "lib-mingw-w64" "lib-static-ucrt" "lib")
        set(POTENTIAL_LIB "${GLFW_DIR}/${LIB_DIR}/glfw3.lib")
        if(EXISTS "${POTENTIAL_LIB}")
            set(GLFW_LIBRARY "${POTENTIAL_LIB}")
            message(STATUS "Found GLFW library at: ${GLFW_LIBRARY}")
            break()
        endif()
    endforeach()
endif()

# Include GLFW directory
include_directories(${GLFW_INCLUDE_DIR})

# Configure ImGui

if(NOT EXISTS "${IMGUI_DIR}/imgui.h")
    message(FATAL_ERROR "Dear ImGui not found. Please clone the Dear ImGui repository to ${IMGUI_DIR}")
endif()

set(IMGUI_SOURCES
    ${IMGUI_DIR}/imgui.cpp
    ${IMGUI_DIR}/imgui_demo.cpp
    ${IMGUI_DIR}/imgui_draw.cpp
    ${IMGUI_DIR}/imgui_tables.cpp
    ${IMGUI_DIR}/imgui_widgets.cpp
    ${IMGUI_DIR}/backends/imgui_impl_sdl2.cpp
    ${IMGUI_DIR}/backends/imgui_impl_sdlrenderer2.cpp
)

# Platform-specific settings
if(WIN32)
    set(PLATFORM_LIBS ws2_32)
    add_definitions(-D_CRT_SECURE_NO_WARNINGS)
elseif(UNIX AND NOT APPLE)
    # Linux
    set(PLATFORM_LIBS dl pthread)
elseif(APPLE)
    # macOS
    set(PLATFORM_LIBS "-framework Cocoa -framework IOKit -framework CoreVideo")
endif()




message(STATUS "SDL: Using ${SDL2_DIR}")
message(STATUS "Using GLFW include dir: ${GLFW_INCLUDE_DIR}")
message(STATUS "Using GLFW library: ${GLFW_LIBRARY}")
message(STATUS "OpenGL libraries: ${OPENGL_LIBRARIES}")
message(STATUS "Using  egfx from: ${EGFX_BASE_DIR}")


#
# Function to create an eGFX simulator test executable
#
# Parameters:
#   TARGET_NAME          - Name of the executable target
#   SCREEN_WIDTH         - Physical screen width in pixels
#   SCREEN_HEIGHT        - Physical screen height in pixels
#   IMG_DEPTH            - Image depth (e.g., "32BPP_XRGB888", "16BPP_RGB565", "1BPP")
#   TEST_SRC_FOLDER      - Path to test source folder containing eGFX_Test.c
#   ASSET_FOLDER         - Path to assets folder for sprite generation
#
# Optional parameters (can be set before calling or passed as named arguments):
#   INITIAL_ZOOM         - Initial window zoom level (default: 1)
#   SIM_GRID_ENABLE      - Enable pixel grid simulation (default: 1)
#   SIM_GRID_PIXEL_SIZE  - Pixel size for grid (default: 4)
#   SIM_GRID_PIXEL_BORDER - Border size between pixels (default: 1)
#   SIM_GRID_BACKGROUND_COLOR - Background color for grid (default: 0x101010)
#   ADDITIONAL_TEST_SOURCES - Additional source files to include
#
function(egfx_add_sim_test TARGET_NAME SCREEN_WIDTH SCREEN_HEIGHT IMG_DEPTH TEST_SRC_FOLDER ASSET_FOLDER)
    # Parse optional arguments
    cmake_parse_arguments(
        ARG
        ""
        "INITIAL_ZOOM;SIM_GRID_ENABLE;SIM_GRID_PIXEL_SIZE;SIM_GRID_PIXEL_BORDER;SIM_GRID_BACKGROUND_COLOR"
        "ADDITIONAL_TEST_SOURCES"
        ${ARGN}
    )

    # Set defaults for optional parameters
    if(NOT DEFINED ARG_INITIAL_ZOOM)
        set(ARG_INITIAL_ZOOM 1)
    endif()
    if(NOT DEFINED ARG_SIM_GRID_ENABLE)
        set(ARG_SIM_GRID_ENABLE 1)
    endif()
    if(NOT DEFINED ARG_SIM_GRID_PIXEL_SIZE)
        set(ARG_SIM_GRID_PIXEL_SIZE 4)
    endif()
    if(NOT DEFINED ARG_SIM_GRID_PIXEL_BORDER)
        set(ARG_SIM_GRID_PIXEL_BORDER 1)
    endif()
    if(NOT DEFINED ARG_SIM_GRID_BACKGROUND_COLOR)
        set(ARG_SIM_GRID_BACKGROUND_COLOR 0x101010)
    endif()

    # Set configuration variables for driver header generation
    set(SIM_IMG_DEPTH "${IMG_DEPTH}")
    set(SIM_IMG_TYPE "EGFX_IMG_${IMG_DEPTH}")
    set(INITIAL_ZOOM ${ARG_INITIAL_ZOOM})
    set(SIM_GRID_ENABLE ${ARG_SIM_GRID_ENABLE})
    set(SIM_GRID_PIXEL_SIZE ${ARG_SIM_GRID_PIXEL_SIZE})
    set(SIM_GRID_PIXEL_BORDER ${ARG_SIM_GRID_PIXEL_BORDER})
    set(SIM_GRID_BACKGROUND_COLOR ${ARG_SIM_GRID_BACKGROUND_COLOR})
    set(SIM_PHYSICAL_SCREEN_SIZE_X ${SCREEN_WIDTH})
    set(SIM_PHYSICAL_SCREEN_SIZE_Y ${SCREEN_HEIGHT})

    # Generate sprites
    generate_sprites(${SIM_IMG_DEPTH} ${ASSET_FOLDER} ${TEST_SRC_FOLDER} "sprites")

    # Configure the ImGui driver header
    configure_file(${EGFX_IO_DIR}/eGFX_DisplayDriver_ImGui.h.in
                   ${TEST_SRC_FOLDER}/eGFX_DisplayDriver_ImGui.h)

    # Set up test sources
    set(TEST_SRC
        "${TEST_SRC_FOLDER}/egfx_test.c"
        "${TEST_SRC_FOLDER}/sprites.c"
        "${EGFX_IO_DIR}/eGFX_DisplayDriver_ImGui.cpp"
        ${IMGUI_SOURCES}
        ${ARG_ADDITIONAL_TEST_SOURCES}
    )

    # Create executable
    add_executable(${TARGET_NAME} ${TEST_SRC})

    # Link libraries
    target_link_libraries(
        ${TARGET_NAME}
        ${SDL2_LIBRARIES}
        ${PLATFORM_LIBS}
    )

    # Include directories
    target_include_directories(
        ${TARGET_NAME} PRIVATE
        "${TEST_SRC_FOLDER}"
        ${eGFX_SRC}
        ${EGFX_IO_DIR}
        ${IMGUI_DIR}
        ${IMGUI_DIR}/backends
        ${OPENGL_INCLUDE_DIR}
    )

    # Compile definitions
    target_compile_definitions(${TARGET_NAME} PRIVATE
        SIMULATOR=1
        SDL_MAIN_HANDLED
    )

    # Add eGFX to target
    egfx__add_to_target(${TARGET_NAME})

    # Set C/C++ standards
    set_target_properties(${TARGET_NAME}
        PROPERTIES
        C_STANDARD 99
        C_STANDARD_REQUIRED ON
        CXX_STANDARD 20
        CXX_STANDARD_REQUIRED ON
    )

    # Windows-specific: Copy SDL DLLs
    if(${CMAKE_SYSTEM_NAME} STREQUAL "Windows")
        add_custom_command(TARGET ${TARGET_NAME} POST_BUILD
            COMMAND ${CMAKE_COMMAND} -E copy_directory
                "${SDL2_DIR}/../lib/x64"
                $<TARGET_FILE_DIR:${TARGET_NAME}>)
    endif()

    message(STATUS "Created eGFX simulator test: ${TARGET_NAME} (${SCREEN_WIDTH}x${SCREEN_HEIGHT}, ${IMG_DEPTH})")
endfunction()
