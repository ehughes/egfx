#ifdef __cplusplus
extern "C" {
#endif

#ifndef eGFX_BLIT_H
#define eGFX_BLIT_H

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

#endif

#ifdef __cplusplus
 }
#endif
