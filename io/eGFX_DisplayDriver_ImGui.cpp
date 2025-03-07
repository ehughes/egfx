// Dear ImGui: standalone example application for SDL2 + SDL_Renderer
// (SDL is a cross-platform general purpose library for handling windows, inputs, OpenGL/Vulkan/Metal graphics context creation, etc.)

// Learn about Dear ImGui:
// - FAQ                  https://dearimgui.com/faq
// - Getting Started      https://dearimgui.com/getting-started
// - Documentation        https://dearimgui.com/docs (same as your local docs/ folder).
// - Introduction, links and more at the top of imgui.cpp

#include "imgui.h"
#include "imgui_impl_sdl2.h"
#include "imgui_impl_sdlrenderer2.h"
#include <stdio.h>
#include <SDL.h>

// Regular includes
#include "eGFX.h"
#include "eGFX_DisplayDriver_ImGui.h" // You might need to update this header for SDL
#include <stdio.h>
#include <string.h>
#include <algorithm> // For std::min and std::max

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

// SDL and ImGui globals
SDL_Window* eGFX_Window = nullptr;
SDL_Renderer* eGFX_Renderer = nullptr;
SDL_Texture* eGFX_Texture = nullptr;
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

// Add this global variable to track when zoom has been changed
bool zoomChanged = false;

void UpdateWindowTitle() {
    snprintf(WindowTitle, sizeof(WindowTitle), "%d x %d  Zoom:%.1f %dBPP",
              eGFX_PHYSICAL_SCREEN_SIZE_X, 
              eGFX_PHYSICAL_SCREEN_SIZE_Y,
              eGFX_Zoom,
              eGFX_IMAGE_PLANE_BITS_PER_PIXEL_FROM_TYPE(eGFX_DISPLAY_DRIVER_IMAGE_PLANE_TYPE)
    );
    
    if (eGFX_Window) {
        SDL_SetWindowTitle(eGFX_Window, WindowTitle);
    }
}

uint32_t eGFX_GetInactiveBackBuffer() {
    return InactiveBackBuffer++;
}

// Process SDL events
int ProcessSimEvents() {
    SDL_Event event;
    
    while (SDL_PollEvent(&event)) {
        ImGui_ImplSDL2_ProcessEvent(&event);
        
        if (event.type == SDL_QUIT) {
            return -1;
        }
        
        // Handle mouse wheel zoom
        if (event.type == SDL_MOUSEWHEEL) {
            if (event.wheel.y > 0) {
                eGFX_Zoom = std::min(4.0f, eGFX_Zoom + 0.25f); // Increment by 0.25, max at 4
            } else if (event.wheel.y < 0) {
                eGFX_Zoom = std::max(1.0f, eGFX_Zoom - 0.25f);  // Decrement by 0.25, min at 1
            }
            
            // Mark that zoom changed
            zoomChanged = true;
            
            // Update the window title with the new zoom value
            UpdateWindowTitle();
        }
    }
    
    return 0;
}

void eGFX_DeInitDriver() {
    // Clean up ImGui and SDL
    ImGui_ImplSDLRenderer2_Shutdown();
    ImGui_ImplSDL2_Shutdown();
    ImGui::DestroyContext();
    
    if (eGFX_Texture) {
        SDL_DestroyTexture(eGFX_Texture);
        eGFX_Texture = nullptr;
    }
    
    if (eGFX_Renderer) {
        SDL_DestroyRenderer(eGFX_Renderer);
        eGFX_Renderer = nullptr;
    }
    
    if (eGFX_Window) {
        SDL_DestroyWindow(eGFX_Window);
        eGFX_Window = nullptr;
    }
    
    SDL_Quit();
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

    // Initialize SDL
    if (SDL_Init(SDL_INIT_VIDEO | SDL_INIT_TIMER | SDL_INIT_GAMECONTROLLER) != 0) {
        printf("Error: %s\n", SDL_GetError());
        return;
    }

    // Create window with SDL
    int window_width = eGFX_PHYSICAL_SCREEN_SIZE_X * TEXTURE_BUFFER_PIXEL_SIZE ;
    int window_height = eGFX_PHYSICAL_SCREEN_SIZE_Y * TEXTURE_BUFFER_PIXEL_SIZE;
    
    // Simple window setup without dark mode hints
    
    SDL_WindowFlags window_flags = (SDL_WindowFlags)(SDL_WINDOW_RESIZABLE);// | SDL_WINDOW_ALLOW_HIGHDPI);
    eGFX_Window = SDL_CreateWindow(WindowTitle, SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, 
                                   window_width, window_height, window_flags);
    
    if (eGFX_Window == nullptr) {
        printf("Error creating window: %s\n", SDL_GetError());
        return;
    }

    // Create renderer
    eGFX_Renderer = SDL_CreateRenderer(eGFX_Window, -1, SDL_RENDERER_PRESENTVSYNC | SDL_RENDERER_ACCELERATED);
    if (eGFX_Renderer == nullptr) {
        printf("Error creating renderer: %s\n", SDL_GetError());
        return;
    }

    // Setup Dear ImGui context
    IMGUI_CHECKVERSION();
    ImGui::CreateContext();
    ImGuiIO& io = ImGui::GetIO();
    io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;

    // Setup Dear ImGui style
    ImGui::StyleColorsDark();

    // Setup Platform/Renderer backends
    ImGui_ImplSDL2_InitForSDLRenderer(eGFX_Window, eGFX_Renderer);
    ImGui_ImplSDLRenderer2_Init(eGFX_Renderer);

    // Clear texture buffer
    memset(TextureBuffer, 0, sizeof(TextureBuffer));

    // Create texture for LCD
    eGFX_Texture = SDL_CreateTexture(
        eGFX_Renderer,
        SDL_PIXELFORMAT_RGBA32,
        SDL_TEXTUREACCESS_STREAMING,
        eGFX_PHYSICAL_SCREEN_SIZE_X * TEXTURE_BUFFER_PIXEL_SIZE,
        eGFX_PHYSICAL_SCREEN_SIZE_Y * TEXTURE_BUFFER_PIXEL_SIZE
    );

    if (eGFX_Texture == nullptr) {
        printf("Error creating texture: %s\n", SDL_GetError());
        return;
    }
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
            
            switch (Image->Type)
			{
			case eGFX_IMAGE_PLANE_1BPP:

				if (PS)
				{
					TexturePixelColor = 0xFFFFFFFF;
				}
				else
				{
					TexturePixelColor = 0;
				}

				break;

			case eGFX_IMAGE_PLANE_4BPP:

				//ToDo  Test Visually

				PS &= 0xF;

				TexturePixelColor = eGFX_COLOR_RGB_TO_RGBX888(PS << 4, PS<<4 , PS << 4);

				break;


			case eGFX_IMAGE_PLANE_8BPP:

				//ToDo  Test Visually

				PS &= 0xFF;

				TexturePixelColor = eGFX_COLOR_RGB_TO_RGBX888(PS, PS ,PS);

				break;


			case eGFX_IMAGE_PLANE_8BPP_XRGB222:

				PS &= 0xFF;

				TexturePixelColor = ((PS & 0x30) >> 4) << (16 + 6) |
									((PS & 0x0C) >> 2) << (8 + 6) |
									((PS & 0x03)) << (6);


				TexturePixelColor += 0x1F1F1F;

				break;


			case eGFX_IMAGE_PLANE_16BPP_RGB565:
				
				PS &= 0xFFFF;

				//ToDo  Test Visually and implement eGFX Macro

				r = (PS>>11) & (0x1F);
				g = (PS>>5) & (0x3F);
				b = (PS) & (0x1F);
				
				if (r)
					r = r << 3 | 0x7;
				if (g)
					g = g << 2 | 0x3;
				if (b)
					b = b << 3 | 0x7;

				TexturePixelColor = eGFX_COLOR_RGB_TO_RGBX888(r,g,b);

				break;

			case eGFX_IMAGE_PLANE_16BPP_BGR565:
				
				PS &= 0xFFFF;

				//ToDo  Test Visually and implement eGFX Macro

				b = (PS >> 11) & (0x1F);
				g = (PS >> 5) & (0x3F);
				r = (PS) & (0x1F);

				TexturePixelColor = eGFX_COLOR_RGB_TO_RGBX888(r, g, b);

				break;

			case eGFX_IMAGE_PLANE_16BPP_GBRG3553:

				PS &= 0xFFFF;

				//ToDo  Test Visually and implement eGFX Macro

				r = (PS >> 3) & (0x1F);
				b = (PS >> 8) & (0x1F);
				g = (PS&0x7) | (PS>>10) ;

				TexturePixelColor = eGFX_COLOR_RGB_TO_RGBX888(r, g, b);

				break;

			case eGFX_IMAGE_PLANE_16BPP_GRBG3553:

				PS &= 0xFFFF;

				//ToDo  Test Visually and implement eGFX Macro

				b = (PS >> 3) & (0x1F);
				r = (PS >> 8) & (0x1F);
				g = (PS & 0x7) | (PS >> 10);

				TexturePixelColor = eGFX_COLOR_RGB_TO_RGBX888(r, g, b);

				break;

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
    
    // Process events
    ProcessSimEvents();
    
    // Start the ImGui frame
    ImGui_ImplSDLRenderer2_NewFrame();
    ImGui_ImplSDL2_NewFrame();
    ImGui::NewFrame();
    
    // Create an ImGui window for the LCD simulation
    ImGui::SetNextWindowPos(ImVec2(0, 0), ImGuiCond_FirstUseEver);
    ImGui::SetNextWindowSize(ImVec2(
        eGFX_PHYSICAL_SCREEN_SIZE_X * TEXTURE_BUFFER_PIXEL_SIZE * eGFX_Zoom + 20, // Add padding
        eGFX_PHYSICAL_SCREEN_SIZE_Y * TEXTURE_BUFFER_PIXEL_SIZE * eGFX_Zoom + 40  // Add padding for title bar, etc.
    ));
    
    ImGui::Begin("LCD Simulator", nullptr, ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoResize);
    
    // Update texture with our pixel data
    SDL_UpdateTexture(
        eGFX_Texture, 
        NULL, 
        TextureBuffer, 
        eGFX_PHYSICAL_SCREEN_SIZE_X * TEXTURE_BUFFER_PIXEL_SIZE * sizeof(uint32_t)
    );
    
    // Calculate texture dimensions for display
    ImVec2 imageSize(
        eGFX_PHYSICAL_SCREEN_SIZE_X * TEXTURE_BUFFER_PIXEL_SIZE * eGFX_Zoom,
        eGFX_PHYSICAL_SCREEN_SIZE_Y * TEXTURE_BUFFER_PIXEL_SIZE * eGFX_Zoom
    );
    
    // Display the texture
    ImGui::Image((ImTextureID)(intptr_t)eGFX_Texture, imageSize);
    
    ImGui::End();
    
    // Rendering
    ImGui::Render();

    // Clear screen
    SDL_SetRenderDrawColor(eGFX_Renderer, 26, 26, 26, 255); // Dark gray background
    SDL_RenderClear(eGFX_Renderer);
    
    // Render ImGui
    ImGui_ImplSDLRenderer2_RenderDrawData(ImGui::GetDrawData(), eGFX_Renderer);
    
    // Present renderer
    SDL_RenderPresent(eGFX_Renderer);
    
    // Call vsync callback if provided
    if (VSyncCallback != NULL) {
        VSyncCallback(Image);
    }
}

void eGFX_SetBacklight(uint8_t BacklightValue) {
    // Implement if needed
}