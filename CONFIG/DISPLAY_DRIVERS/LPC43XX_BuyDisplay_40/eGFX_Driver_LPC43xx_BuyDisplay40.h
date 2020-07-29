#include "Core/eGFX_DataTypes.h"

#ifndef eGFX_DRIVER_LPC43XX_BUY_DISPLAY_40
#define eGFX_DRIVER_LPC43XX_BUY_DISPLAY_40

#define eGFX_DISPLAY_DRIVER_PRESENT

#define eGFX_PHYSICAL_SCREEN_SIZE_X     ((uint16_t) 480)
#define eGFX_PHYSICAL_SCREEN_SIZE_Y     ((uint16_t) 480)


/*
    eGFX_InitDriver() should do any hardware related init for the display and
    setup eGFX_BackBuffer
*/

extern void  eGFX_InitDriver();

/*
    eGFX_WaitForV_Sync() should block until the the last dump operation is complete or to when it is OK to do another Dump
*/

extern void eGFX_WaitForV_Sync();

/*
    eGFX_Dump() should dump an ImagePlane to the physical screen.
*/

void  eGFX_Dump(eGFX_ImagePlane *Image);

#define eGFX_NUM_BACKBUFFERS	2

extern eGFX_ImagePlane eGFX_BackBuffer[eGFX_NUM_BACKBUFFERS];

/*
 *  Integer value 0 (off) to 100 (full on)
 *
 */

extern void eGFX_SetBacklight(uint8_t BacklightValue);


uint32_t eGFX_V_SyncReady();


#endif

