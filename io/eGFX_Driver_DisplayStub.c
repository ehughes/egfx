#include "../../eGFX.h"

#ifdef eGFX_DRIVER_STUB

eGFX_ImagePlane eGFX_BackBuffer;

uint8_t BackBufferStore[EGFX_CALC_4BPP_IMG_STORAGE_SPACE_SIZE(eGFX_PHYSICAL_SCREEN_SIZE_X,eGFX_PHYSICAL_SCREEN_SIZE_Y)];

egfx_vsync_callback_t *VSyncCallback;

void eGFX_InitDriver(egfx_vsync_callback_t VS)
{
    egfx_image_plane_init(&eGFX_BackBuffer,
                          BackBufferStore,
                          eGFX_PHYSICAL_SCREEN_SIZE_X,
                          eGFX_PHYSICAL_SCREEN_SIZE_Y,
                          EGFX_IMG_4BPP);


    VSyncCallback = VS;
  
}

void eGFX_Dump(eGFX_ImagePlane *Image)
{

    if (VSyncCallback != NULL)
	{
		VSyncCallback(Image);
	}

}

void eGFX_SetBacklight(uint8_t BacklightValue)
{



}

#endif
