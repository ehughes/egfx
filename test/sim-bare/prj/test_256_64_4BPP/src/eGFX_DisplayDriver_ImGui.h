#include "egfx_types.h"

#ifndef EGFX_DRIVER_IMGUI
#define EGFX_DRIVER_IMGUI

/*
    ImGui Driver Specific Options
*/

/*
    The window used to render the simulated display can be enlarged for small screens, etc
    This is the default zoom level
*/
#define EGFX_IMGUI_INITIAL_ZOOM  (1)

/*
    For smaller matrix LED displays, OLEDs, etc there can be a small gap between pixels.
    EGFX_IMGUI_SIM_GRID render the pixels as rectangles with some empty space in between.

    Note that the zoom level gets applied to this. The actual space used by each pixel is PIXEL_SIZE + 2*EGFX_IMGUI_SIM_GRID_PIXEL_BORDER
*/
#define EGFX_IMGUI_SIM_GRID      (1)

#if EGFX_IMGUI_SIM_GRID == 1
    #define EGFX_IMGUI_SIM_GRID_PIXEL_SIZE        (4)
    #define EGFX_IMGUI_SIM_GRID_PIXEL_BORDER      (1)
    #define EGFX_IMGUI_SIM_GRID_BACKGROUND_COLOR  (0x101010)
#endif



/*
     Required symbols for any driver
*/
#define EGFX_DISPLAY_DRIVER_PRESENT

#define EGFX_DISPLAY_DRIVER_IMG_TYPE   EGFX_IMG_4BPP

#define EGFX_PHYSICAL_SCREEN_SIZE_X     ((uint16_t) 256)
#define EGFX_PHYSICAL_SCREEN_SIZE_Y     ((uint16_t) 64)

/*
    egfx_init_driver() should do any hardware related init for the display and
    setup egfx_back_buffer
*/
#ifdef __cplusplus
extern "C" {
#endif

/*
    Function to process ImGui and GLFW events
*/
extern int ProcessSimEvents();

extern void egfx_init_driver(egfx_vsync_callback_t VS);

/*
   egfx_deinit_driver() is provided in case you want some way of shutting down the graphics
*/
extern void egfx_deinit_driver();

/*
    egfx_dump() should dump an ImagePlane to the physical screen.
*/
extern void egfx_dump(egfx_img *Image);

/*
    A driver should expose at least one back buffer that is the physical screen size and have
    a matching color space.
*/
#define EGFX_NUM_BACKBUFFERS 2

extern egfx_img* egfx_back_buffer[EGFX_NUM_BACKBUFFERS];

extern uint32_t egfx_get_inactive_back_buffer();

/*
*  Integer value 0 (off) to 100 (full on)
*/
extern void egfx_set_backlight(uint8_t BacklightValue);

#ifdef __cplusplus
}
#endif

#endif
