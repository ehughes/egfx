// Dear ImGui: standalone example application for SDL2 + SDL_Renderer
// (SDL is a cross-platform general purpose library for handling windows, inputs, OpenGL/Vulkan/Metal graphics context creation, etc.)

// Learn about Dear ImGui:
// - FAQ                  https://dearimgui.com/faq
// - Getting Started      https://dearimgui.com/getting-started
// - Documentation        https://dearimgui.com/docs (same as your local docs/ folder).
// - Introduction, links and more at the top of imgui.cpp

#ifndef SDL_MAIN_HANDLED
#define SDL_MAIN_HANDLED
#endif

#include "imgui.h"
#include "imgui_impl_sdl2.h"
#include "imgui_impl_sdlrenderer2.h"
#include <stdio.h>
#include <SDL.h>

// Regular includes
#include "egfx.h"
#include "egfx_pixel_ops.h" // For raw pixel access functions
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
// Using static constructor macro to create the texture plane with its storage
EGFX_IMG_MAKE(TexturePlane,
              eGFX_PHYSICAL_SCREEN_SIZE_X * TEXTURE_BUFFER_PIXEL_SIZE,
              eGFX_PHYSICAL_SCREEN_SIZE_Y * TEXTURE_BUFFER_PIXEL_SIZE,
              EGFX_IMG_32BPP_XRGB888);

// SDL and ImGui globals
SDL_Window* eGFX_Window = nullptr;
SDL_Renderer* eGFX_Renderer = nullptr;
SDL_Texture* eGFX_Texture = nullptr;
float eGFX_Zoom = 1.0f;
bool eGFX_ShouldClose = false;
char WindowTitle[64];

// Create backbuffers using static constructor macro
#if eGFX_NUM_BACKBUFFERS == 2
EGFX_IMG_MAKE(eGFX_BackBuffer_0, eGFX_PHYSICAL_SCREEN_SIZE_X, eGFX_PHYSICAL_SCREEN_SIZE_Y, eGFX_DISPLAY_DRIVER_IMG_TYPE);
EGFX_IMG_MAKE(eGFX_BackBuffer_1, eGFX_PHYSICAL_SCREEN_SIZE_X, eGFX_PHYSICAL_SCREEN_SIZE_Y, eGFX_DISPLAY_DRIVER_IMG_TYPE);

// Array of pointers to backbuffers for compatibility
egfx_img* eGFX_BackBuffer[eGFX_NUM_BACKBUFFERS] = {
    &eGFX_BackBuffer_0,
    &eGFX_BackBuffer_1
};
#else
#error "Only 2 backbuffers are currently supported with static initialization"
#endif

static uint32_t InactiveBackBuffer = 0;
egfx_vsync_callback_t *VSyncCallback;

// Add this global variable to track when zoom has been changed
bool zoomChanged = false;

void UpdateWindowTitle() {
    snprintf(WindowTitle, sizeof(WindowTitle), "%d x %d  Zoom:%.1f %dBPP",
              eGFX_PHYSICAL_SCREEN_SIZE_X, 
              eGFX_PHYSICAL_SCREEN_SIZE_Y,
              eGFX_Zoom,
              EGFX_IMG_BPP_FROM_TYPE(eGFX_DISPLAY_DRIVER_IMG_TYPE)
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

void eGFX_InitDriver(egfx_vsync_callback_t VS) {
    VSyncCallback = VS;

    // Clear back buffers (already statically initialized)
    memset(eGFX_BackBuffer_0_storage, 0, sizeof(eGFX_BackBuffer_0_storage));
    memset(eGFX_BackBuffer_1_storage, 0, sizeof(eGFX_BackBuffer_1_storage));

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
    int window_width = eGFX_PHYSICAL_SCREEN_SIZE_X * TEXTURE_BUFFER_PIXEL_SIZE * eGFX_IMGUI_INITIAL_ZOOM * 1.5;
    int window_height = eGFX_PHYSICAL_SCREEN_SIZE_Y * TEXTURE_BUFFER_PIXEL_SIZE * eGFX_IMGUI_INITIAL_ZOOM * 1.5;
    
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
    memset(TexturePlane_storage, 0, sizeof(TexturePlane_storage));

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

void eGFX_Dump(egfx_img *Image) 
{
    egfx_pixel_state PS;
    int r, g, b;
    uint32_t TexturePixelColor = 0;
    
    // Clear texture buffer for grid mode
    #if (eGFX_IMGUI_SIM_GRID == 1)
        egfx_box PixelBox;
        uint32_t* TextureBuffer = (uint32_t*)TexturePlane_storage;
        for (int k = 0; k < sizeof(TexturePlane_storage) / sizeof(uint32_t); k++) {
            TextureBuffer[k] = eGFX_IMGUI_SIM_GRID_BACKGROUND_COLOR;
        }
    #endif
    
    // Process each pixel from the image plane
    // Use raw pixel access (no transformations) since we're reading physical memory layout
    for (int y = 0; y < eGFX_PHYSICAL_SCREEN_SIZE_Y; y++) {
        for (int x = 0; x < eGFX_PHYSICAL_SCREEN_SIZE_X; x++) {
            // Read pixel directly from physical memory (bypass all transformations)
            switch (EGFX_GET_BPP_FROM_IMG(Image)) {
                case 1:
                    PS = egfx_gp_1bpp(Image->data, Image->size_x, x, y);
                    break;
                case 3:
                    PS = egfx_gp_3bpp(Image->data, Image->size_x, x, y);
                    break;
                case 4:
                    PS = egfx_gp_4bpp(Image->data, Image->size_x, x, y);
                    break;
                case 8:
                    PS = egfx_gp_8bpp(Image->data, Image->size_x, x, y);
                    break;
                case 16:
                    PS = egfx_gp_16bpp(Image->data, Image->size_x, x, y);
                    break;
                case 24:
                    PS = egfx_gp_24bpp(Image->data, Image->size_x, x, y);
                    break;
                case 32:
                    PS = egfx_gp_32bpp(Image->data, Image->size_x, x, y);
                    break;
                default:
                    PS = 0;
                    break;
            }
            
            // Convert pixel based on image type
            r = g = b = 0;
            
            switch (EGFX_IMG_GET_TYPE(Image))
			{
			case EGFX_IMG_1BPP:

				if (PS)
				{
					TexturePixelColor = 0xFFFFFFFF;
				}
				else
				{
					TexturePixelColor = 0;
				}

				break;

			case EGFX_IMG_4BPP:

				//ToDo  Test Visually

				PS &= 0xF;

				TexturePixelColor = EGFX_COLOR_RGB_TO_BGRX888(PS << 4, PS<<4 , PS << 4);

				break;


			case EGFX_IMG_8BPP:

				//ToDo  Test Visually

				PS &= 0xFF;

				TexturePixelColor = EGFX_COLOR_RGB_TO_BGRX888(PS, PS ,PS);

				break;


			case EGFX_IMG_8BPP_XRGB222:


				PS &= 0xFF;

                
				r = ((PS>>4) & (0x1F)) << 6;
				g = ((PS>>2) & (0x1F)) << 6;
			    b = (PS & (0x1F)) << 6;
				
				TexturePixelColor = EGFX_COLOR_RGB_TO_BGRX888(r,g,b);

				break;


			case EGFX_IMG_16BPP_RGB565:
				
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

				TexturePixelColor = EGFX_COLOR_RGB_TO_BGRX888(r,g,b);

				break;

			case EGFX_IMG_16BPP_BGR565:
				
				PS &= 0xFFFF;

				//ToDo  Test Visually and implement eGFX Macro

				b = (PS >> 11) & (0x1F);
				g = (PS >> 5) & (0x3F);
				r = (PS) & (0x1F);

				TexturePixelColor = EGFX_COLOR_RGB_TO_BGRX888(r, g, b);

				break;

			case EGFX_IMG_16BPP_GBRG3553:

				PS &= 0xFFFF;


				r = (PS >> 3) & (0x1F);
				b = (PS >> 8) & (0x1F);
				g = (PS&0x7) | (PS>>10) ;

				TexturePixelColor = EGFX_COLOR_RGB_TO_BGRX888(r, g, b);

				break;

			case EGFX_IMG_16BPP_GRBG3553:

				PS &= 0xFFFF;

				b = (PS >> 3) & (0x1F);
				r = (PS >> 8) & (0x1F);
				g = (PS & 0x7) | (PS >> 10);

				TexturePixelColor = EGFX_COLOR_RGB_TO_BGRX888(r, g, b);

				break;

			case EGFX_IMG_32BPP:
			case EGFX_IMG_32BPP_XRGB888:

				r = (PS>>16) & (0xFF);
				g = (PS>>8) & (0xFF);
				b = (PS) & (0xFF);

               	TexturePixelColor = EGFX_COLOR_RGB_TO_BGRX888(r, g, b);
			
				break;
            }


            // Draw pixel to texture buffer
            #if (eGFX_IMGUI_SIM_GRID == 1)
                PixelBox.p1.x = eGFX_IMGUI_SIM_GRID_PIXEL_BORDER + (x * (TEXTURE_BUFFER_PIXEL_SIZE));
                PixelBox.p1.y = eGFX_IMGUI_SIM_GRID_PIXEL_BORDER + (y * (TEXTURE_BUFFER_PIXEL_SIZE));
                PixelBox.p2.x = PixelBox.p1.x + eGFX_IMGUI_SIM_GRID_PIXEL_SIZE;
                PixelBox.p2.y = PixelBox.p1.y + eGFX_IMGUI_SIM_GRID_PIXEL_SIZE;
                eGFX_DrawFilledBox(&TexturePlane, &PixelBox, TexturePixelColor);
            #else
                ((uint32_t*)TexturePlane_storage)[y * eGFX_PHYSICAL_SCREEN_SIZE_X + x] = TexturePixelColor;
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
        TexturePlane_storage,
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