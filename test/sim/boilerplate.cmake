
set(TOOLS_DIR "${EGFX_DIR}/tools")
set(eGFX_SRC "${EGFX_DIR}/src")
set(GLFW_DIR "${EGFX_DIR}/test/sim/glfw-3.4.bin.WIN64")
set(IMGUI_DIR "${EGFX_DIR}/test/sim/imgui")


if(${CMAKE_SYSTEM_NAME} STREQUAL "Windows")
    message(STATUS "Configuring SDL for Windows")
    set(SDL2_DIR "${EGFX_DIR}/test/sim/sdl2/cmake")
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
message(STATUS "Using  egfx from: ${EGFX_DIR}")


