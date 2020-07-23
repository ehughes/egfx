#include "../../eGFX.h"


#ifndef eGFX_DRIVER_169160128ASC2__NRF52840
#define eGFX_DRIVER_169160128ASC2__NRF52840

	#define eGFX_DISPLAY_DRIVER_PRESENT

	#define eGFX_PHYSICAL_SCREEN_SIZE_X     ((uint16_t) 160)
	#define eGFX_PHYSICAL_SCREEN_SIZE_Y     ((uint16_t) 128)

	extern void eGFX_InitDriver();

	extern void eGFX_WaitForV_Sync();

	extern void eGFX_Dump(eGFX_ImagePlane *Image);

	extern eGFX_ImagePlane eGFX_BackBuffer;

	extern void eGFX_SetBacklight(uint8_t BacklightValue);

#endif
