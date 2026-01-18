// Dear ImGui: standalone example application for GLFW + OpenGL 3, using programmable pipeline
// (GLFW is a cross-platform general purpose library for handling windows, inputs, OpenGL/Vulkan/Metal graphics context creation, etc.)

// Learn about Dear ImGui:
// - FAQ                  https://dearimgui.com/faq
// - Getting Started      https://dearimgui.com/getting-started
// - Documentation        https://dearimgui.com/docs (same as your local docs/ folder).
// - Introduction, links and more at the top of imgui.cpp

#include "imgui.h"
#include "imgui_impl_glfw.h"
#include "imgui_impl_opengl3.h"
#include <stdio.h>
#define GL_SILENCE_DEPRECATION
#if defined(IMGUI_IMPL_OPENGL_ES2)
#include <GLES2/gl2.h>
#endif
#include <GLFW/glfw3.h> // Will drag system OpenGL headers

// [Win32] Our example includes a copy of glfw3.lib pre-compiled with VS2010 to maximize ease of testing and compatibility with old VS compilers.
// To link with VS2010-era libraries, VS2015+ requires linking with legacy_stdio_definitions.lib, which we do using this pragma.
// Your own project should not be affected, as you are likely to link with a newer binary of GLFW that is adequate for your version of Visual Studio.
#if defined(_MSC_VER) && (_MSC_VER >= 1900) && !defined(IMGUI_DISABLE_WIN32_FUNCTIONS)
#pragma comment(lib, "legacy_stdio_definitions")
#endif

// This example can also compile and run with Emscripten! See 'Makefile.emscripten' for details.
#ifdef __EMSCRIPTEN__
#include "../libs/emscripten/emscripten_mainloop_stub.h"
#endif

// Regular includes
#include "egfx.h"
#include "eGFX_DisplayDriver_ImGui.h"
#include <stdio.h>
#include <string.h>


#if EGFX_IMGUI_SIM_GRID == 1
#define TEXTURE_BUFFER_PIXEL_SIZE (EGFX_IMGUI_SIM_GRID_PIXEL_SIZE + (EGFX_IMGUI_SIM_GRID_PIXEL_BORDER*2))
#else
#define TEXTURE_BUFFER_PIXEL_SIZE 1
#endif

// Texture buffer for our simulated LCD
uint32_t TextureBuffer[EGFX_PHYSICAL_SCREEN_SIZE_X * EGFX_PHYSICAL_SCREEN_SIZE_Y * TEXTURE_BUFFER_PIXEL_SIZE * TEXTURE_BUFFER_PIXEL_SIZE];

eGFX_ImagePlane TexturePlane =
{
    .type = EGFX_IMG_32BPP_XRGB888,
    .Data = (uint8_t *)TextureBuffer,
    .SizeX = EGFX_PHYSICAL_SCREEN_SIZE_X * TEXTURE_BUFFER_PIXEL_SIZE,
    .SizeY = EGFX_PHYSICAL_SCREEN_SIZE_Y * TEXTURE_BUFFER_PIXEL_SIZE,
    .User = NULL
};

// GLFW and ImGui globals
GLFWwindow* egfx_window;
GLuint egfx_texture_id;
float egfx_zoom = 1.0f;
bool egfx_should_close = false;
char WindowTitle[64];

eGFX_ImagePlane egfx_back_buffer[EGFX_NUM_BACKBUFFERS];

// Frame buffer definitions based on bit depth
#if (EGFX_IMG_BPP_FROM_TYPE(EGFX_DISPLAY_DRIVER_IMG_TYPE) == 1)
uint8_t egfx_frame_buffer[EGFX_NUM_BACKBUFFERS][EGFX_CALC_1BPP_IMG_STORAGE_SPACE_SIZE(EGFX_PHYSICAL_SCREEN_SIZE_X, EGFX_PHYSICAL_SCREEN_SIZE_Y)];
#elif (EGFX_IMG_BPP_FROM_TYPE(EGFX_DISPLAY_DRIVER_IMG_TYPE) == 3)
uint8_t egfx_frame_buffer[EGFX_CALC_3BPP_IMG_STORAGE_SPACE_SIZE(EGFX_PHYSICAL_SCREEN_SIZE_X, EGFX_PHYSICAL_SCREEN_SIZE_Y)];
#elif (EGFX_IMG_BPP_FROM_TYPE(EGFX_DISPLAY_DRIVER_IMG_TYPE) == 4)
uint8_t egfx_frame_buffer[EGFX_NUM_BACKBUFFERS][EGFX_CALC_4BPP_IMG_STORAGE_SPACE_SIZE(EGFX_PHYSICAL_SCREEN_SIZE_X, EGFX_PHYSICAL_SCREEN_SIZE_Y)];
#elif (EGFX_IMG_BPP_FROM_TYPE(EGFX_DISPLAY_DRIVER_IMG_TYPE) == 8)
uint8_t egfx_frame_buffer[EGFX_NUM_BACKBUFFERS][EGFX_CALC_8BPP_IMG_STORAGE_SPACE_SIZE(EGFX_PHYSICAL_SCREEN_SIZE_X, EGFX_PHYSICAL_SCREEN_SIZE_Y)];
#elif (EGFX_IMG_BPP_FROM_TYPE(EGFX_DISPLAY_DRIVER_IMG_TYPE) == 16)
uint8_t egfx_frame_buffer[EGFX_NUM_BACKBUFFERS][EGFX_CALC_16BPP_IMG_STORAGE_SPACE_SIZE(EGFX_PHYSICAL_SCREEN_SIZE_X, EGFX_PHYSICAL_SCREEN_SIZE_Y)];
#elif (EGFX_IMG_BPP_FROM_TYPE(EGFX_DISPLAY_DRIVER_IMG_TYPE) == 24)
uint8_t egfx_frame_buffer[EGFX_NUM_BACKBUFFERS][EGFX_CALC_24BPP_IMG_STORAGE_SPACE_SIZE(EGFX_PHYSICAL_SCREEN_SIZE_X, EGFX_PHYSICAL_SCREEN_SIZE_Y)];
#elif (EGFX_IMG_BPP_FROM_TYPE(EGFX_DISPLAY_DRIVER_IMG_TYPE) == 32)
uint8_t egfx_frame_buffer[EGFX_NUM_BACKBUFFERS][EGFX_CALC_32BPP_IMG_STORAGE_SPACE_SIZE(EGFX_PHYSICAL_SCREEN_SIZE_X, EGFX_PHYSICAL_SCREEN_SIZE_Y)];
#else
#error "I need a valid backbuffer image plane type"
#endif

static uint32_t InactiveBackBuffer = 0;
egfx_vsync_callback_t *VSyncCallback;

// Error callback for GLFW
static void glfw_error_callback(int error, const char* description) {
    fprintf(stderr, "GLFW Error %d: %s\n", error, description);
}

void UpdateWindowTitle() {
    snprintf(WindowTitle, sizeof(WindowTitle), "%d x %d  Zoom:%.1f %dBPP",
              EGFX_PHYSICAL_SCREEN_SIZE_X,
              EGFX_PHYSICAL_SCREEN_SIZE_Y,
              egfx_zoom,
              EGFX_IMG_BPP_FROM_TYPE(EGFX_DISPLAY_DRIVER_IMG_TYPE)
    );
}

uint32_t egfx_get_inactive_back_buffer() {
    return InactiveBackBuffer++;
}

// Process ImGui and GLFW events
int ProcessSimEvents() {
    // Poll and handle events
    glfwPollEvents();

    if (glfwWindowShouldClose(egfx_window)) {
        return -1;
    }

    return 0;
}

void egfx_deinit_driver() {
    // Clean up ImGui and GLFW
    ImGui_ImplOpenGL3_Shutdown();
    ImGui_ImplGlfw_Shutdown();
    ImGui::DestroyContext();

    if (egfx_texture_id) {
        glDeleteTextures(1, &egfx_texture_id);
    }

    glfwDestroyWindow(egfx_window);
    glfwTerminate();
}


#define GL_CALL(x) x

// Add this global variable to track when zoom has been changed
bool zoomChanged = false;
#include <algorithm>  // For std::min and std::max
// Add this scroll callback function definition at the top of your file
void scroll_callback(GLFWwindow* window, double xoffset, double yoffset)
{
    // Wheel up (yoffset > 0) increases zoom, wheel down decreases
    if (yoffset > 0)
        egfx_zoom = std::min(4.0f, egfx_zoom + .25f); // Increment by 1, max at 10
    else if (yoffset < 0)
        egfx_zoom = std::max(1.0f, egfx_zoom - .25f);  // Decrement by 1, min at 1

    // Mark that zoom changed
    zoomChanged = true;

    // Update the window title with the new zoom value
    UpdateWindowTitle();
    glfwSetWindowTitle(egfx_window, WindowTitle);
}

void egfx_init_driver(egfx_vsync_callback_t VS) {
    VSyncCallback = VS;

    // Initialize back buffers
    for (int i = 0; i < EGFX_NUM_BACKBUFFERS; i++) {
        egfx_image_plane_init(&egfx_back_buffer[i],
            &egfx_frame_buffer[i][0],
            EGFX_PHYSICAL_SCREEN_SIZE_X,
            EGFX_PHYSICAL_SCREEN_SIZE_Y,
            EGFX_DISPLAY_DRIVER_IMG_TYPE);
    }

    memset(egfx_frame_buffer, 0, sizeof(egfx_frame_buffer));

    // Set initial zoom
#ifndef EGFX_IMGUI_INITIAL_ZOOM
    egfx_zoom = 640.0f / EGFX_PHYSICAL_SCREEN_SIZE_X;
#else
    egfx_zoom = EGFX_IMGUI_INITIAL_ZOOM;
    if (egfx_zoom < 1.0f) {
        egfx_zoom = 1.0f / EGFX_PHYSICAL_SCREEN_SIZE_X;
    }
#endif

    // Setup window title
    UpdateWindowTitle();

    // Setup GLFW error callback
    glfwSetErrorCallback(glfw_error_callback);
    if (!glfwInit())
        return;

    // Decide GL+GLSL versions
    const char* glsl_version = "#version 130";
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 0);

    // Create window with graphics context
    egfx_window = glfwCreateWindow(

         EGFX_PHYSICAL_SCREEN_SIZE_X * TEXTURE_BUFFER_PIXEL_SIZE *1.5 ,
         EGFX_PHYSICAL_SCREEN_SIZE_Y * TEXTURE_BUFFER_PIXEL_SIZE *1.5,

        WindowTitle, NULL, NULL);

    if (egfx_window == NULL)
        return;

    glfwMakeContextCurrent(egfx_window);
    glfwSwapInterval(1); // Enable vsync

   if (egfx_window != NULL) {
        // Register scroll callback
        glfwSetScrollCallback(egfx_window, scroll_callback);
    }

    // Setup Dear ImGui context
    IMGUI_CHECKVERSION();
    ImGui::CreateContext();
    ImGuiIO& io = ImGui::GetIO();
    io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;

    // Setup Dear ImGui style
    ImGui::StyleColorsDark();

    // Scale ImGui for high DPI displays
    float xscale, yscale;
    glfwGetWindowContentScale(egfx_window, &xscale, &yscale);
    if (xscale > 1.0f) {
        io.FontGlobalScale = xscale;
    }

    // Setup Platform/Renderer backends
    ImGui_ImplGlfw_InitForOpenGL(egfx_window, true);
    ImGui_ImplOpenGL3_Init(glsl_version);

    // Clear texture buffer
    memset(TextureBuffer, 0, sizeof(TextureBuffer));

    GLint last_texture;

    GL_CALL(glGetIntegerv(GL_TEXTURE_BINDING_2D, &last_texture));

    glGenTextures(1, &egfx_texture_id);
    glBindTexture(GL_TEXTURE_2D, egfx_texture_id);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);

#ifdef GL_UNPACK_ROW_LENGTH // Not on WebGL/ES
    glPixelStorei(GL_UNPACK_ROW_LENGTH, 0);
#endif
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA,
        EGFX_PHYSICAL_SCREEN_SIZE_X * TEXTURE_BUFFER_PIXEL_SIZE,
        EGFX_PHYSICAL_SCREEN_SIZE_Y * TEXTURE_BUFFER_PIXEL_SIZE,
        0, GL_RGBA, GL_UNSIGNED_BYTE, TextureBuffer);


    GL_CALL(glBindTexture(GL_TEXTURE_2D, last_texture));

}
 void egfx_dump(eGFX_ImagePlane *Image) {
    egfx_pixel_state PS;
    int r, g, b;
    uint32_t TexturePixelColor = 0;
    
    


    // Clear texture buffer for grid mode
    #if (EGFX_IMGUI_SIM_GRID == 1)
        egfx_box PixelBox;
        for (int k = 0; k < sizeof(TextureBuffer) / sizeof(uint32_t); k++) {
            TextureBuffer[k] = EGFX_IMGUI_SIM_GRID_BACKGROUND_COLOR;
        }
    #endif
    
    // Process each pixel from the image plane
    for (int y = 0; y < EGFX_PHYSICAL_SCREEN_SIZE_Y; y++) {
        for (int x = 0; x < EGFX_PHYSICAL_SCREEN_SIZE_X; x++) {
            PS = egfx_get_pixel(Image, x, y);
            
            // Convert pixel based on image type
            r = g = b = 0;
            
            switch (Image->type) {
                case EGFX_IMG_1BPP:
                    TexturePixelColor = PS ? 0xFFFFFFFF : 0xFF000000;
                    break;
                    
                case EGFX_IMG_4BPP:
                    PS &= 0xF;
                    TexturePixelColor = 0xFF000000 | EGFX_COLOR_RGB_TO_RGBX888(PS << 4, PS << 4, PS << 4);
                    break;
                    
                // [other pixel conversions as in your original code]
                
                case EGFX_IMG_32BPP:
                case EGFX_IMG_32BPP_XRGB888:
                    TexturePixelColor = PS | 0xFF000000;
                    break;
            }
            
            // Draw pixel to texture buffer
            #if (EGFX_IMGUI_SIM_GRID == 1)
                PixelBox.p1.x = EGFX_IMGUI_SIM_GRID_PIXEL_BORDER + (x * (TEXTURE_BUFFER_PIXEL_SIZE));
                PixelBox.p1.y = EGFX_IMGUI_SIM_GRID_PIXEL_BORDER + (y * (TEXTURE_BUFFER_PIXEL_SIZE));
                PixelBox.p2.x = PixelBox.p1.x + EGFX_IMGUI_SIM_GRID_PIXEL_SIZE;
                PixelBox.p2.y = PixelBox.p1.y + EGFX_IMGUI_SIM_GRID_PIXEL_SIZE;
                egfx_draw_filled_box(&TexturePlane, &PixelBox, TexturePixelColor);
            #else
                TextureBuffer[y * EGFX_PHYSICAL_SCREEN_SIZE_X + x] = TexturePixelColor;
            #endif
        }
    }
    
    // Start the ImGui frame
    ImGui_ImplOpenGL3_NewFrame();
    ImGui_ImplGlfw_NewFrame();
    ImGui::NewFrame();
    
    // Create an ImGui window for the LCD simulation
    ImGui::SetNextWindowPos(ImVec2(0, 0), ImGuiCond_FirstUseEver);
ImGui::SetNextWindowSize(ImVec2(
    EGFX_PHYSICAL_SCREEN_SIZE_X * TEXTURE_BUFFER_PIXEL_SIZE * egfx_zoom + 20, // Add padding
    EGFX_PHYSICAL_SCREEN_SIZE_Y * TEXTURE_BUFFER_PIXEL_SIZE * egfx_zoom + 40  // Add padding for title bar, etc.
));
    
    ImGui::Begin("LCD Simulator", nullptr, ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoResize);
    
    // Update texture with our pixel data
    glBindTexture(GL_TEXTURE_2D, egfx_texture_id);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, 
                EGFX_PHYSICAL_SCREEN_SIZE_X * TEXTURE_BUFFER_PIXEL_SIZE ,
                EGFX_PHYSICAL_SCREEN_SIZE_Y * TEXTURE_BUFFER_PIXEL_SIZE,
                0, GL_RGBA, GL_UNSIGNED_BYTE, TextureBuffer);
    
    // Display the texture
    ImVec2 windowSize = ImGui::GetContentRegionAvail();

    ImVec2 imageSize(
    EGFX_PHYSICAL_SCREEN_SIZE_X * TEXTURE_BUFFER_PIXEL_SIZE * egfx_zoom,
    EGFX_PHYSICAL_SCREEN_SIZE_Y * TEXTURE_BUFFER_PIXEL_SIZE * egfx_zoom
);

    //ImVec2 uv_min = ImVec2(0.0f, 0.0f);
   // ImVec2 uv_max = ImVec2(1.0f, 1.0f);
    ImGui::Image((ImTextureID)(intptr_t)egfx_texture_id, imageSize);// , uv_min, uv_max);
    

    
    ImGui::End();
    
    // Rendering
    ImGui::Render();
    int display_w, display_h;
    glfwGetFramebufferSize(egfx_window, &display_w, &display_h);
    glViewport(0, 0, display_w, display_h);
    glClearColor(0.1f, 0.1f, 0.1f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT);
    ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
    
    glfwSwapBuffers(egfx_window);
    
    // Call vsync callback if provided
    if (VSyncCallback != NULL) {
        VSyncCallback(Image);
    }
}


void egfx_set_backlight(uint8_t BacklightValue) {
    // Implement if needed
}