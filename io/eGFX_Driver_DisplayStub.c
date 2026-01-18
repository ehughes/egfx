#include "../../eGFX.h"

#ifdef EGFX_DRIVER_STUB

eGFX_ImagePlane egfx_back_buffer;

uint8_t BackBufferStore[EGFX_CALC_4BPP_IMG_STORAGE_SPACE_SIZE(EGFX_PHYSICAL_SCREEN_SIZE_X,EGFX_PHYSICAL_SCREEN_SIZE_Y)];

egfx_vsync_callback_t *VSyncCallback;

void egfx_init_driver(egfx_vsync_callback_t VS)
{
    egfx_image_plane_init(&egfx_back_buffer,
                          BackBufferStore,
                          EGFX_PHYSICAL_SCREEN_SIZE_X,
                          EGFX_PHYSICAL_SCREEN_SIZE_Y,
                          EGFX_IMG_4BPP);


    VSyncCallback = VS;

}

void egfx_dump(eGFX_ImagePlane *Image)
{

    if (VSyncCallback != NULL)
	{
		VSyncCallback(Image);
	}

}

void egfx_set_backlight(uint8_t BacklightValue)
{



}

#endif
