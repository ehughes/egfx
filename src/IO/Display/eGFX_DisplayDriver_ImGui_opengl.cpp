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
#include "eGFX.h"
#include "eGFX_DisplayDriver_ImGui.h"
#include <stdio.h>
#include <string.h>


#if eGFX_IMGUI_SIM_GRID == 1
#define TEXTURE_BUFFER_PIXEL_SIZE (eGFX_IMGUI_SIM_GRID_PIXEL_SIZE + (eGFX_IMGUI_SIM_GRID_PIXEL_BORDER*2))
#else
#define TEXTURE_BUFFER_PIXEL_SIZE 1
#endif

// Texture buffer for our simulated LCD
uint32_t TextureBuffer[eGFX_PHYSICAL_SCREEN_SIZE_X * eGFX_PHYSICAL_SCREEN_SIZE_Y * TEXTURE_BUFFER_PIXEL_SIZE * TEXTURE_BUFFER_PIXEL_SIZE];

eGFX_ImagePlane TexturePlane = 
{
    .Type = eGFX_IMAGE_PLANE_32BPP_XRGB888,
    .Data = (uint8_t *)TextureBuffer,
    .SizeX = eGFX_PHYSICAL_SCREEN_SIZE_X * TEXTURE_BUFFER_PIXEL_SIZE,
    .SizeY = eGFX_PHYSICAL_SCREEN_SIZE_Y * TEXTURE_BUFFER_PIXEL_SIZE,
    .User = NULL
};

// GLFW and ImGui globals
GLFWwindow* eGFX_Window;
GLuint eGFX_TextureID;
float eGFX_Zoom = 1.0f;
bool eGFX_ShouldClose = false;
char WindowTitle[64];

eGFX_ImagePlane eGFX_BackBuffer[eGFX_NUM_BACKBUFFERS];

// Frame buffer definitions based on bit depth
#if (eGFX_IMAGE_PLANE_BITS_PER_PIXEL_FROM_TYPE(eGFX_DISPLAY_DRIVER_IMAGE_PLANE_TYPE) == 1)
uint8_t eGFX_FrameBuffer[eGFX_NUM_BACKBUFFERS][eGFX_CALCULATE_1BPP_IMAGE_STORAGE_SPACE_SIZE(eGFX_PHYSICAL_SCREEN_SIZE_X, eGFX_PHYSICAL_SCREEN_SIZE_Y)];
#elif (eGFX_IMAGE_PLANE_BITS_PER_PIXEL_FROM_TYPE(eGFX_DISPLAY_DRIVER_IMAGE_PLANE_TYPE) == 3)
uint8_t eGFX_FrameBuffer[eGFX_CALCULATE_3BPP_IMAGE_STORAGE_SPACE_SIZE(eGFX_PHYSICAL_SCREEN_SIZE_X, eGFX_PHYSICAL_SCREEN_SIZE_Y)];
#elif (eGFX_IMAGE_PLANE_BITS_PER_PIXEL_FROM_TYPE(eGFX_DISPLAY_DRIVER_IMAGE_PLANE_TYPE) == 4)
uint8_t eGFX_FrameBuffer[eGFX_NUM_BACKBUFFERS][eGFX_CALCULATE_4BPP_IMAGE_STORAGE_SPACE_SIZE(eGFX_PHYSICAL_SCREEN_SIZE_X, eGFX_PHYSICAL_SCREEN_SIZE_Y)];
#elif (eGFX_IMAGE_PLANE_BITS_PER_PIXEL_FROM_TYPE(eGFX_DISPLAY_DRIVER_IMAGE_PLANE_TYPE) == 8)
uint8_t eGFX_FrameBuffer[eGFX_NUM_BACKBUFFERS][eGFX_CALCULATE_8BPP_IMAGE_STORAGE_SPACE_SIZE(eGFX_PHYSICAL_SCREEN_SIZE_X, eGFX_PHYSICAL_SCREEN_SIZE_Y)];
#elif (eGFX_IMAGE_PLANE_BITS_PER_PIXEL_FROM_TYPE(eGFX_DISPLAY_DRIVER_IMAGE_PLANE_TYPE) == 16)
uint8_t eGFX_FrameBuffer[eGFX_NUM_BACKBUFFERS][eGFX_CALCULATE_16BPP_IMAGE_STORAGE_SPACE_SIZE(eGFX_PHYSICAL_SCREEN_SIZE_X, eGFX_PHYSICAL_SCREEN_SIZE_Y)];
#elif (eGFX_IMAGE_PLANE_BITS_PER_PIXEL_FROM_TYPE(eGFX_DISPLAY_DRIVER_IMAGE_PLANE_TYPE) == 24)
uint8_t eGFX_FrameBuffer[eGFX_NUM_BACKBUFFERS][eGFX_CALCULATE_24BPP_IMAGE_STORAGE_SPACE_SIZE(eGFX_PHYSICAL_SCREEN_SIZE_X, eGFX_PHYSICAL_SCREEN_SIZE_Y)];
#elif (eGFX_IMAGE_PLANE_BITS_PER_PIXEL_FROM_TYPE(eGFX_DISPLAY_DRIVER_IMAGE_PLANE_TYPE) == 32)
uint8_t eGFX_FrameBuffer[eGFX_NUM_BACKBUFFERS][eGFX_CALCULATE_32BPP_IMAGE_STORAGE_SPACE_SIZE(eGFX_PHYSICAL_SCREEN_SIZE_X, eGFX_PHYSICAL_SCREEN_SIZE_Y)];
#else 
#error "I need a valid backbuffer image plane type"
#endif

static uint32_t InactiveBackBuffer = 0;
eGFX_VSyncCallback_t *VSyncCallback;

// Error callback for GLFW
static void glfw_error_callback(int error, const char* description) {
    fprintf(stderr, "GLFW Error %d: %s\n", error, description);
}

void UpdateWindowTitle() {
    snprintf(WindowTitle, sizeof(WindowTitle), "%d x %d  Zoom:%.1f %dBPP",
              eGFX_PHYSICAL_SCREEN_SIZE_X, 
              eGFX_PHYSICAL_SCREEN_SIZE_Y,
              eGFX_Zoom,
              eGFX_IMAGE_PLANE_BITS_PER_PIXEL_FROM_TYPE(eGFX_DISPLAY_DRIVER_IMAGE_PLANE_TYPE)
    );
}

uint32_t eGFX_GetInactiveBackBuffer() {
    return InactiveBackBuffer++;
}

// Process ImGui and GLFW events
int ProcessSimEvents() {
    // Poll and handle events
    glfwPollEvents();
    
    if (glfwWindowShouldClose(eGFX_Window)) {
        return -1;
    }
    
    return 0;
}

void eGFX_DeInitDriver() {
    // Clean up ImGui and GLFW
    ImGui_ImplOpenGL3_Shutdown();
    ImGui_ImplGlfw_Shutdown();
    ImGui::DestroyContext();
    
    if (eGFX_TextureID) {
        glDeleteTextures(1, &eGFX_TextureID);
    }
    
    glfwDestroyWindow(eGFX_Window);
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
        eGFX_Zoom = std::min(4.0f, eGFX_Zoom + .25f); // Increment by 1, max at 10
    else if (yoffset < 0)
        eGFX_Zoom = std::max(1.0f, eGFX_Zoom - .25f);  // Decrement by 1, min at 1
    
    // Mark that zoom changed
    zoomChanged = true;
    
    // Update the window title with the new zoom value
    UpdateWindowTitle();
    glfwSetWindowTitle(eGFX_Window, WindowTitle);
}

void eGFX_InitDriver(eGFX_VSyncCallback_t VS) {
    VSyncCallback = VS;

    // Initialize back buffers
    for (int i = 0; i < eGFX_NUM_BACKBUFFERS; i++) {
        eGFX_ImagePlaneInit(&eGFX_BackBuffer[i],
            &eGFX_FrameBuffer[i][0],
            eGFX_PHYSICAL_SCREEN_SIZE_X,
            eGFX_PHYSICAL_SCREEN_SIZE_Y,
            eGFX_DISPLAY_DRIVER_IMAGE_PLANE_TYPE);
    }

    memset(eGFX_FrameBuffer, 0, sizeof(eGFX_FrameBuffer));

    // Set initial zoom
#ifndef eGFX_IMGUI_INITIAL_ZOOM
    eGFX_Zoom = 640.0f / eGFX_PHYSICAL_SCREEN_SIZE_X;
#else
    eGFX_Zoom = eGFX_IMGUI_INITIAL_ZOOM;
    if (eGFX_Zoom < 1.0f) {
        eGFX_Zoom = 1.0f / eGFX_PHYSICAL_SCREEN_SIZE_X;
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
    eGFX_Window = glfwCreateWindow(

         eGFX_PHYSICAL_SCREEN_SIZE_X * TEXTURE_BUFFER_PIXEL_SIZE *1.5 ,
         eGFX_PHYSICAL_SCREEN_SIZE_Y * TEXTURE_BUFFER_PIXEL_SIZE *1.5,

        WindowTitle, NULL, NULL);

    if (eGFX_Window == NULL)
        return;

    glfwMakeContextCurrent(eGFX_Window);
    glfwSwapInterval(1); // Enable vsync

   if (eGFX_Window != NULL) {
        // Register scroll callback
        glfwSetScrollCallback(eGFX_Window, scroll_callback);
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
    glfwGetWindowContentScale(eGFX_Window, &xscale, &yscale);
    if (xscale > 1.0f) {
        io.FontGlobalScale = xscale;
    }

    // Setup Platform/Renderer backends
    ImGui_ImplGlfw_InitForOpenGL(eGFX_Window, true);
    ImGui_ImplOpenGL3_Init(glsl_version);

    // Clear texture buffer
    memset(TextureBuffer, 0, sizeof(TextureBuffer));

    GLint last_texture;

    GL_CALL(glGetIntegerv(GL_TEXTURE_BINDING_2D, &last_texture));

    glGenTextures(1, &eGFX_TextureID);
    glBindTexture(GL_TEXTURE_2D, eGFX_TextureID);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);

#ifdef GL_UNPACK_ROW_LENGTH // Not on WebGL/ES
    glPixelStorei(GL_UNPACK_ROW_LENGTH, 0);
#endif
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA,
        eGFX_PHYSICAL_SCREEN_SIZE_X * TEXTURE_BUFFER_PIXEL_SIZE,
        eGFX_PHYSICAL_SCREEN_SIZE_Y * TEXTURE_BUFFER_PIXEL_SIZE,
        0, GL_RGBA, GL_UNSIGNED_BYTE, TextureBuffer);


    GL_CALL(glBindTexture(GL_TEXTURE_2D, last_texture));

}
 void eGFX_Dump(eGFX_ImagePlane *Image) {
    eGFX_PixelState PS;
    int r, g, b;
    uint32_t TexturePixelColor = 0;
    
    


    // Clear texture buffer for grid mode
    #if (eGFX_IMGUI_SIM_GRID == 1)
        eGFX_Box PixelBox;
        for (int k = 0; k < sizeof(TextureBuffer) / sizeof(uint32_t); k++) {
            TextureBuffer[k] = eGFX_IMGUI_SIM_GRID_BACKGROUND_COLOR;
        }
    #endif
    
    // Process each pixel from the image plane
    for (int y = 0; y < eGFX_PHYSICAL_SCREEN_SIZE_Y; y++) {
        for (int x = 0; x < eGFX_PHYSICAL_SCREEN_SIZE_X; x++) {
            PS = eGFX_GetPixel(Image, x, y);
            
            // Convert pixel based on image type
            r = g = b = 0;
            
            switch (Image->Type) {
                case eGFX_IMAGE_PLANE_1BPP:
                    TexturePixelColor = PS ? 0xFFFFFFFF : 0xFF000000;
                    break;
                    
                case eGFX_IMAGE_PLANE_4BPP:
                    PS &= 0xF;
                    TexturePixelColor = 0xFF000000 | eGFX_COLOR_RGB_TO_RGBX888(PS << 4, PS << 4, PS << 4);
                    break;
                    
                // [other pixel conversions as in your original code]
                
                case eGFX_IMAGE_PLANE_32BPP:
                case eGFX_IMAGE_PLANE_32BPP_XRGB888:
                    TexturePixelColor = PS | 0xFF000000;
                    break;
            }
            
            // Draw pixel to texture buffer
            #if (eGFX_IMGUI_SIM_GRID == 1)
                PixelBox.P1.X = eGFX_IMGUI_SIM_GRID_PIXEL_BORDER + (x * (TEXTURE_BUFFER_PIXEL_SIZE));
                PixelBox.P1.Y = eGFX_IMGUI_SIM_GRID_PIXEL_BORDER + (y * (TEXTURE_BUFFER_PIXEL_SIZE));
                PixelBox.P2.X = PixelBox.P1.X + eGFX_IMGUI_SIM_GRID_PIXEL_SIZE;
                PixelBox.P2.Y = PixelBox.P1.Y + eGFX_IMGUI_SIM_GRID_PIXEL_SIZE;
                eGFX_DrawFilledBox(&TexturePlane, &PixelBox, TexturePixelColor);
            #else
                TextureBuffer[y * eGFX_PHYSICAL_SCREEN_SIZE_X + x] = TexturePixelColor;
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
    eGFX_PHYSICAL_SCREEN_SIZE_X * TEXTURE_BUFFER_PIXEL_SIZE * eGFX_Zoom + 20, // Add padding
    eGFX_PHYSICAL_SCREEN_SIZE_Y * TEXTURE_BUFFER_PIXEL_SIZE * eGFX_Zoom + 40  // Add padding for title bar, etc.
));
    
    ImGui::Begin("LCD Simulator", nullptr, ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoResize);
    
    // Update texture with our pixel data
    glBindTexture(GL_TEXTURE_2D, eGFX_TextureID);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, 
                eGFX_PHYSICAL_SCREEN_SIZE_X * TEXTURE_BUFFER_PIXEL_SIZE ,
                eGFX_PHYSICAL_SCREEN_SIZE_Y * TEXTURE_BUFFER_PIXEL_SIZE,
                0, GL_RGBA, GL_UNSIGNED_BYTE, TextureBuffer);
    
    // Display the texture
    ImVec2 windowSize = ImGui::GetContentRegionAvail();

    ImVec2 imageSize(
    eGFX_PHYSICAL_SCREEN_SIZE_X * TEXTURE_BUFFER_PIXEL_SIZE * eGFX_Zoom,
    eGFX_PHYSICAL_SCREEN_SIZE_Y * TEXTURE_BUFFER_PIXEL_SIZE * eGFX_Zoom
);

    //ImVec2 uv_min = ImVec2(0.0f, 0.0f);
   // ImVec2 uv_max = ImVec2(1.0f, 1.0f);
    ImGui::Image((ImTextureID)(intptr_t)eGFX_TextureID, imageSize);// , uv_min, uv_max);
    

    
    ImGui::End();
    
    // Rendering
    ImGui::Render();
    int display_w, display_h;
    glfwGetFramebufferSize(eGFX_Window, &display_w, &display_h);
    glViewport(0, 0, display_w, display_h);
    glClearColor(0.1f, 0.1f, 0.1f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT);
    ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
    
    glfwSwapBuffers(eGFX_Window);
    
    // Call vsync callback if provided
    if (VSyncCallback != NULL) {
        VSyncCallback(Image);
    }
}


void eGFX_SetBacklight(uint8_t BacklightValue) {
    // Implement if needed
}