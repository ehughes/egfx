#ifdef __cplusplus
extern "C" {
#endif

#ifndef EGFX_TEXT_H
#define EGFX_TEXT_H

#include "egfx_blit.h"

typedef struct
{
	const egfx_font *font;
	bool colored;
	egfx_rgb888 color;
	bool antialiased;  // If true, use alpha blending (8bpp glyphs); if false, use masked color replacement (1bpp glyphs)
} egfx_text_config;

void EGFX_WEAK egfx_text(egfx_img *image,
                                 char *string,
                                 egfx_point position,
                                 const egfx_text_config *config);

#endif

#ifdef __cplusplus
}
#endif
