cmake_minimum_required(VERSION 3.10)

project(egfx_test_256_64_4BPP)

message(STATUS "Cmake System name is ${CMAKE_SYSTEM_NAME}")

# Configure eGFX
if(NOT DEFINED EGFX_DIR)
    set(EGFX_DIR "${CMAKE_CURRENT_LIST_DIR}/../../../../")
    message(STATUS "Using egfx base of: ${EGFX_DIR}")
endif()

if(NOT DEFINED TARGET)
    set(TARGET ${PROJECT_NAME})
endif()

include("${EGFX_DIR}/test/sim/boilerplate.cmake")

set(EGFX_CONFIG_LOC "${CMAKE_CURRENT_LIST_DIR}/src")
set(ASSET_LOC "${CMAKE_CURRENT_LIST_DIR}/assets")
set(TEST_SRC_FOLDER "${CMAKE_CURRENT_LIST_DIR}/src")


# Options for the ImGui Driver
set(SIM_IMAGE_PLANE_DEPTH "4BPP")
set(SIM_IMAGE_PLANE_TYPE "eGFX_IMAGE_PLANE_${SIM_IMAGE_PLANE_DEPTH}")
set(INITIAL_ZOOM 1)
set(SIM_GRID_ENABLE 1)
set(SIM_GRID_PIXEL_BORDER 1)
set(SIM_GRID_PIXEL_SIZE 4)
set(SIM_GRID_BACKGROUND_COLOR 0x101010)
set(SIM_PHYSICAL_SCREEN_SIZE_X 256)
set(SIM_PHYSICAL_SCREEN_SIZE_Y 64)


#generate our local assets

function(generate_sprites)
    if(CMAKE_SYSTEM_NAME STREQUAL "Windows")
        set(CMD "${TOOLS_DIR}/eGFX_Tools.exe")
    else()
        set(CMD "mono" "${TOOLS_DIR}/eGFX_Tools.exe")
    endif()
    
    message(STATUS "Generating sprite files during configuration...")
    
    execute_process(
        COMMAND ${CMD} "GenerateSprites" "${ASSET_LOC}" "eGFX_IMAGE_PLANE_${SIM_IMAGE_PLANE_DEPTH}" "${TEST_SRC_FOLDER}"
        RESULT_VARIABLE RESULT_CODE
        OUTPUT_VARIABLE TOOL_OUTPUT
        ERROR_VARIABLE TOOL_ERROR
    )
    
    if(NOT RESULT_CODE EQUAL 0)
        message(FATAL_ERROR "Sprite generation failed: ${TOOL_ERROR}")
    else()
        message(STATUS "Successfully generated sprite files")
    endif()
endfunction()

# Call the function during configuration
generate_sprites()

# Configure the ImGui driver header
configure_file(${eGFX_IO}/eGFX_DisplayDriver_ImGui.h.in
               ${CMAKE_CURRENT_LIST_DIR}/src/eGFX_DisplayDriver_ImGui.h)

set(TEST_SRC    
    "${TEST_SRC_FOLDER}/eGFX_Test.c"
    "${TEST_SRC_FOLDER}/Sprites_${SIM_IMAGE_PLANE_DEPTH}.c" 
    "${eGFX_IO}/eGFX_DisplayDriver_ImGui.cpp"
    ${IMGUI_SOURCES}
)

add_executable(${TARGET} ${TEST_SRC})

# Link libraries
target_link_libraries(
    ${TARGET}
    #${OPENGL_LIBRARIES}   Only for opengl imgui backend
    #${GLFW_LIBRARY}
    ${SDL2_LIBRARIES}
    ${PLATFORM_LIBS}
)

target_include_directories(
    ${TARGET} PRIVATE
    "${CMAKE_CURRENT_LIST_DIR}/src"
    ${eGFX_SRC}
	${eGFX_IO}
    ${IMGUI_DIR}
    ${IMGUI_DIR}/backends
    ${OPENGL_INCLUDE_DIR}
)

target_compile_definitions(${TARGET} PRIVATE
    SIMULATOR=1
)

add_subdirectory(${eGFX_SRC} egfx)

set_target_properties(${TARGET} 
    PROPERTIES
    C_STANDARD 99
    C_STANDARD_REQUIRED ON
    CXX_STANDARD 20
    CXX_STANDARD_REQUIRED ON
)

if(${CMAKE_SYSTEM_NAME} STREQUAL "Windows")
    #needed for visual studio outputs.  Need to make sure the SDL dll's exist next to the binary
    add_custom_command(TARGET ${TARGET} POST_BUILD
        COMMAND ${CMAKE_COMMAND} -E copy_directory
            "${SDL2_DIR}/../lib/x64"
            $<TARGET_FILE_DIR:${TARGET}>)
endif()