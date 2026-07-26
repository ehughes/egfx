#ifdef __cplusplus
extern "C" {
#endif

#ifndef EGFX_BLIT_H
#define EGFX_BLIT_H

#include "egfx_color.h"

typedef enum
{
	EGFX_BLIT_MODE_REGULAR = 0,
	EGFX_BLIT_MODE_MASKED,
	EGFX_BLIT_MODE_COLORED,
	EGFX_BLIT_MODE_ALPHA_BLENDED,
} egfx_blit_mode;

typedef struct
{
	const egfx_img *dest_img;
	const egfx_img *source_img;
	egfx_point dest_point;
	egfx_rect src_rect;
	egfx_blit_mode blit_mode;
	egfx_rgb888 color;
} egfx_blit_config;

void EGFX_WEAK egfx_blit(const egfx_img *dest,
                         const egfx_img *src,
                         egfx_point destination_point);

void EGFX_WEAK egfx_blit_ex(const egfx_blit_config *config);

// Blit with fade - scales pixel values toward zero
// fade: 0 = all black, 255 = full brightness
// Works with grayscale formats (1BPP, 4BPP, 8BPP) - scales intensity
// For color formats, scales each channel proportionally
void EGFX_WEAK egfx_blit_faded(egfx_img *dest,
                               const egfx_img *src,
                               egfx_point position,
                               uint8_t fade);

#endif

#ifdef __cplusplus
 }
#endif
