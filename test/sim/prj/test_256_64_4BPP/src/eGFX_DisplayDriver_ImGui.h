#include "eGFX_DataTypes.h"

#ifndef eGFX_DRIVER_IMGUI
#define eGFX_DRIVER_IMGUI

/*
    ImGui Driver Specific Options
*/

/*
    The window used to render the simulated display can be enlarged for small screens, etc
    This is the default zoom level
*/
#define eGFX_IMGUI_INITIAL_ZOOM  (1)

/*
    For smaller matrix LED displays, OLEDs, etc there can be a small gap between pixels.
    eGFX_IMGUI_SIM_GRID render the pixels as rectangles with some empty space in between.  

    Note that the zoom level gets applied to this. The actual space used by each pixel is PIXEL_SIZE + 2*eGFX_IMGUI_SIM_GRID_PIXEL_BORDER
*/
#define eGFX_IMGUI_SIM_GRID      (1)

#if eGFX_IMGUI_SIM_GRID == 1
    #define eGFX_IMGUI_SIM_GRID_PIXEL_SIZE        (4)
    #define eGFX_IMGUI_SIM_GRID_PIXEL_BORDER      (1)
    #define eGFX_IMGUI_SIM_GRID_BACKGROUND_COLOR  (0x101010)
#endif



/*
     Required symbols for any driver
*/
#define eGFX_DISPLAY_DRIVER_PRESENT

#define eGFX_DISPLAY_DRIVER_IMAGE_PLANE_TYPE   eGFX_IMAGE_PLANE_4BPP

#define eGFX_PHYSICAL_SCREEN_SIZE_X     ((uint16_t) 256)
#define eGFX_PHYSICAL_SCREEN_SIZE_Y     ((uint16_t) 64)

/*
    eGFX_InitDriver() should do any hardware related init for the display and
    setup eGFX_BackBuffer
*/
#ifdef __cplusplus
extern "C" {
#endif

/*
    Function to process ImGui and GLFW events
*/
extern int ProcessSimEvents();

extern void eGFX_InitDriver(eGFX_VSyncCallback_t VS);

/*
   eGFX_DeInitDriver() is provided in case you want some way of shutting down the graphics
*/
extern void eGFX_DeInitDriver();

/*
    eGFX_Dump() should dump an ImagePlane to the physical screen.
*/
extern void eGFX_Dump(eGFX_ImagePlane *Image);

/*
    A driver should expose at least one back buffer that is the physical screen size and have
    a matching color space.
*/
#define eGFX_NUM_BACKBUFFERS 2

extern eGFX_ImagePlane eGFX_BackBuffer[eGFX_NUM_BACKBUFFERS];

extern uint32_t eGFX_GetInactiveBackBuffer();

/*
*  Integer value 0 (off) to 100 (full on)
*/
extern void eGFX_SetBacklight(uint8_t BacklightValue);

#ifdef __cplusplus
}
#endif

#endif
