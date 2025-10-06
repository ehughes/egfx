#include "egfx.h"

#if CONFIG_EGFX_INCLUDE_BITMAP_OUTPUT_SUPPORT == 1 

#ifndef _eGFX_BMP
#define _eGFX_BMP

void ImagePlaneToGrayScaleBMP(char *FileName, egfx_img * IP);

#endif

#endif