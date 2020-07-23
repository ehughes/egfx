#include "..\..\eGFX.h"

#ifndef eGFX_DRIVER_27_MEM_LCD__LPC43xx
#define eGFX_DRIVER_27_MEM_LCD__LPC43xx

	#define eGFX_DISPLAY_DRIVER_PRESENT

	#define eGFX_PHYSICAL_SCREEN_SIZE_X	((uint16_t) 400)	//This is the actual X and Y size of the physical screen in *pixels*
	#define eGFX_PHYSICAL_SCREEN_SIZE_Y	((uint16_t) 240)

	extern void	eGFX_InitDriver();

	extern void	eGFX_Dump(eGFX_ImagePlane *Image);

	extern eGFX_ImagePlane eGFX_BackBuffer;

	extern void eGFX_SetBacklight(uint8_t BacklightValue);

	extern void eGFX_WaitForV_Sync();

#endif


