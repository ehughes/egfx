#include "../../eGFX.h"

#ifdef eGFX_DRIVER_STUB

eGFX_ImagePlane eGFX_BackBuffer;

uint8_t BackBufferStore[eGFX_CALCULATE_4BPP_IMAGE_STORAGE_SPACE_SIZE(eGFX_PHYSICAL_SCREEN_SIZE_X,eGFX_PHYSICAL_SCREEN_SIZE_Y)];

void eGFX_InitDriver()
{
    eGFX_ImagePlaneInit(&eGFX_BackBuffer,
                        BackBufferStore,
                        eGFX_PHYSICAL_SCREEN_SIZE_X,
                        eGFX_PHYSICAL_SCREEN_SIZE_Y,
                        eGFX_IMAGE_PLANE_4BPP);
  
}


void eGFX_WaitForV_Sync()
{
    
}


void eGFX_Dump(eGFX_ImagePlane *Image)
{



}

void eGFX_SetBacklight(uint8_t BacklightValue)
{



}

#endif
