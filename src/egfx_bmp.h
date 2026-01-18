#include "egfx.h"

#if CONFIG_EGFX_INCLUDE_BITMAP_OUTPUT_SUPPORT == 1 

#ifndef EGFX_BMP_H
#define EGFX_BMP_H

void ImagePlaneToGrayScaleBMP(char *FileName, egfx_img * IP);

#endif

#endif