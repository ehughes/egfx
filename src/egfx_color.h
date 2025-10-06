#include "stdint.h"

#ifndef EGFX_COLOR_H
#define EGFX_COLOR_H

/***
 *            ____ _______  __   ____      _
 *       ___ / ___|  ___\ \/ /  / ___|___ | | ___  _ __ ___
 *      / _ \ |  _| |_   \  /  | |   / _ \| |/ _ \| '__/ __|
 *     |  __/ |_| |  _|  /  \  | |__| (_) | | (_) | |  \__ \
 *      \___|\____|_|   /_/\_\  \____\___/|_|\___/|_|  |___/
 *
 */

// RGB888 color type (8 bits per channel)
typedef struct {
    uint8_t r;
    uint8_t g;
    uint8_t b;
} egfx_rgb888;

// ARGB color type for alpha blending (integer, 0-255 range)
typedef struct {
    uint8_t a;
    uint8_t r;
    uint8_t g;
    uint8_t b;
} egfx_argb;

// ARGB color type for alpha blending (floating-point, 0.0-1.0 range)
typedef struct {
    float a;
    float r;
    float g;
    float b;
} egfx_argb_f;

// RGB888 to RGB565 conversion
#define EGFX_COLOR_RGB888_TO_RGB565(R,G,B)     ( ((R>>3)<<11)   | ((G>>2)<<5)   | (B>>3)   )

// RGB to RGBX888 conversion
#define EGFX_COLOR_RGB_TO_RGBX888(R,G,B)       ( ((R&0xff)<<16) | ((G&0xff)<<8) | (B&0xff) )
#define EGFX_COLOR_RGB_TO_BGRX888(R,G,B)       ( ((B&0xff)<<16) | ((G&0xff)<<8) | (R&0xff) )

// RGB565 component extraction
#define EGFX_RGB565_TO_R8(rgb565)  ((((rgb565) >> 11) & 0x1F) << 3)
#define EGFX_RGB565_TO_G8(rgb565)  ((((rgb565) >> 5) & 0x3F) << 2)
#define EGFX_RGB565_TO_B8(rgb565)  (((rgb565) & 0x1F) << 3)

// Fast Approx of Luminance from rgb565
#define EGFX_RGB565_TO_Y8(rgb565)		( \
											( \
											   (((uint16_t)EGFX_RGB565_TO_R8(rgb565))*77) + \
											   (((uint16_t)EGFX_RGB565_TO_G8(rgb565))*150) + \
											   (((uint16_t)EGFX_RGB565_TO_B8(rgb565))*29)) \
											) >> 8 \
										)

// Fast Approx of Luminance to rgb565
#define EGFX_Y8_TO_RGB565(Y8)		(((((uint16_t)(Y8))>>3)<<11) |  \
									((((uint16_t)(Y8))>>2)<<5)   |  \
									((((uint16_t)(Y8))>>3)))

// Pixel state constants
#define EGFX_PIXEL_ON  				    0xFFFFFFFF
#define EGFX_PIXEL_OFF    				0

// 3BPP RGB color constants
#define EGFX_3BPP_RGB__BLACK			0
#define EGFX_3BPP_RGB__RED				1
#define EGFX_3BPP_RGB__GREEN			2
#define EGFX_3BPP_RGB__BLUE				4
#define EGFX_3BPP_RGB__YELLOW			3
#define EGFX_3BPP_RGB__CYAN				6
#define EGFX_3BPP_RGB__MAGENTA			5
#define EGFX_3BPP_RGB__WHITE			7

// 4BPP XBGR color constants
#define EGFX_4BPP_XBGR__BLACK			0
#define EGFX_4BPP_XBGR__RED				1
#define EGFX_4BPP_XBGR__GREEN			2
#define EGFX_4BPP_XBGR__BLUE			4
#define EGFX_4BPP_XBGR__YELLOW			3
#define EGFX_4BPP_XBGR__CYAN			6
#define EGFX_4BPP_XBGR__MAGENTA			5
#define EGFX_4BPP_XBGR__WHITE			7

#endif // EGFX_COLOR_H

// Color conversion functions (requires egfx_types.h to be included first)
#if defined(eGFX_DATA_TYPES_H) && !defined(EGFX_COLOR_FUNCTIONS_H)
#define EGFX_COLOR_FUNCTIONS_H

// Alpha blending helper: blend foreground with background using alpha
// Formula: (fg * alpha + bg * (255 - alpha)) >> 8
static inline uint8_t egfx_alpha_blend_component(uint8_t fg, uint8_t bg, uint8_t alpha)
{
	return (fg * alpha + bg * (255 - alpha)) >> 8;
}

// Convert pixel_state to ARGB based on image type
static inline egfx_argb egfx_pixel_to_argb(egfx_pixel_state pixel, uint16_t img_type)
{
	egfx_argb result;
	result.a = 255;  // Opaque by default

	switch (img_type)
	{
		case EGFX_IMG_16BPP_RGB565:
			result.r = EGFX_RGB565_TO_R8(pixel);
			result.g = EGFX_RGB565_TO_G8(pixel);
			result.b = EGFX_RGB565_TO_B8(pixel);
			break;

		case EGFX_IMG_32BPP_XRGB888:
			result.r = (pixel >> 16) & 0xFF;
			result.g = (pixel >> 8) & 0xFF;
			result.b = pixel & 0xFF;
			break;

		case EGFX_IMG_8BPP_XRGB222:
			// XRGB222: 2 bits per channel
			result.r = ((pixel >> 4) & 0x03) * 85;  // 0-3 -> 0-255
			result.g = ((pixel >> 2) & 0x03) * 85;
			result.b = (pixel & 0x03) * 85;
			break;

		case EGFX_IMG_4BPP:
			// Grayscale
			{
				uint8_t gray = (pixel & 0x0F) * 17;  // 0-15 -> 0-255
				result.r = gray;
				result.g = gray;
				result.b = gray;
			}
			break;

		default:
			// Fallback: treat as grayscale
			result.r = pixel & 0xFF;
			result.g = pixel & 0xFF;
			result.b = pixel & 0xFF;
			break;
	}

	return result;
}

// Convert ARGB back to pixel_state based on image type
static inline egfx_pixel_state egfx_argb_to_pixel(egfx_argb argb, uint16_t img_type)
{
	switch (img_type)
	{
		case EGFX_IMG_16BPP_RGB565:
			return EGFX_COLOR_RGB888_TO_RGB565(argb.r, argb.g, argb.b);

		case EGFX_IMG_32BPP_XRGB888:
			return EGFX_COLOR_RGB_TO_RGBX888(argb.r, argb.g, argb.b);

		case EGFX_IMG_8BPP_XRGB222:
			// XRGB222: 2 bits per channel
			return ((argb.r >> 6) << 4) | ((argb.g >> 6) << 2) | (argb.b >> 6);

		case EGFX_IMG_4BPP:
			// Grayscale: simple average
			{
				uint8_t gray = (argb.r + argb.g + argb.b) / 3;
				return gray >> 4;  // Scale to 4-bit
			}

		default:
			// Fallback: grayscale
			return (argb.r + argb.g + argb.b) / 3;
	}
}

// Saturate float value to 0.0-1.0 range
static inline float egfx_saturate_f(float value)
{
	if (value < 0.0f) return 0.0f;
	if (value > 1.0f) return 1.0f;
	return value;
}

// Convert pixel_state to floating-point ARGB (0.0-1.0 range) based on image type
static inline egfx_argb_f egfx_pixel_to_argb_f(egfx_pixel_state pixel, uint16_t img_type)
{
	egfx_argb_f result;
	result.a = 1.0f;  // Opaque by default

	switch (img_type)
	{
		case EGFX_IMG_16BPP_RGB565:
			// RGB565: 5 bits R, 6 bits G, 5 bits B
			result.r = (float)(((pixel >> 11) & 0x1F)) / 31.0f;
			result.g = (float)(((pixel >> 5) & 0x3F)) / 63.0f;
			result.b = (float)((pixel & 0x1F)) / 31.0f;
			break;

		case EGFX_IMG_32BPP_XRGB888:
			result.r = (float)((pixel >> 16) & 0xFF) / 255.0f;
			result.g = (float)((pixel >> 8) & 0xFF) / 255.0f;
			result.b = (float)(pixel & 0xFF) / 255.0f;
			break;

		case EGFX_IMG_8BPP_XRGB222:
			// XRGB222: 2 bits per channel
			result.r = (float)((pixel >> 4) & 0x03) / 3.0f;
			result.g = (float)((pixel >> 2) & 0x03) / 3.0f;
			result.b = (float)(pixel & 0x03) / 3.0f;
			break;

		case EGFX_IMG_4BPP:
			// Grayscale
			{
				float gray = (float)(pixel & 0x0F) / 15.0f;
				result.r = gray;
				result.g = gray;
				result.b = gray;
			}
			break;

		default:
			// Fallback: treat as 8-bit grayscale
			{
				float gray = (float)(pixel & 0xFF) / 255.0f;
				result.r = gray;
				result.g = gray;
				result.b = gray;
			}
			break;
	}

	return result;
}

// Convert floating-point ARGB (0.0-1.0 range) back to pixel_state with saturation
static inline egfx_pixel_state egfx_argb_f_to_pixel(egfx_argb_f argb, uint16_t img_type)
{
	// Saturate all channels to 0.0-1.0 range
	float r = egfx_saturate_f(argb.r);
	float g = egfx_saturate_f(argb.g);
	float b = egfx_saturate_f(argb.b);

	switch (img_type)
	{
		case EGFX_IMG_16BPP_RGB565:
			// RGB565: 5 bits R, 6 bits G, 5 bits B
			{
				uint16_t r5 = (uint16_t)(r * 31.0f + 0.5f);
				uint16_t g6 = (uint16_t)(g * 63.0f + 0.5f);
				uint16_t b5 = (uint16_t)(b * 31.0f + 0.5f);
				return (r5 << 11) | (g6 << 5) | b5;
			}

		case EGFX_IMG_32BPP_XRGB888:
			{
				uint32_t r8 = (uint32_t)(r * 255.0f + 0.5f);
				uint32_t g8 = (uint32_t)(g * 255.0f + 0.5f);
				uint32_t b8 = (uint32_t)(b * 255.0f + 0.5f);
				return (r8 << 16) | (g8 << 8) | b8;
			}

		case EGFX_IMG_8BPP_XRGB222:
			// XRGB222: 2 bits per channel
			{
				uint8_t r2 = (uint8_t)(r * 3.0f + 0.5f);
				uint8_t g2 = (uint8_t)(g * 3.0f + 0.5f);
				uint8_t b2 = (uint8_t)(b * 3.0f + 0.5f);
				return (r2 << 4) | (g2 << 2) | b2;
			}

		case EGFX_IMG_4BPP:
			// Grayscale: simple average
			{
				float gray = (r + g + b) / 3.0f;
				return (uint8_t)(gray * 15.0f + 0.5f);  // Scale to 4-bit
			}

		default:
			// Fallback: grayscale
			{
				float gray = (r + g + b) / 3.0f;
				return (uint8_t)(gray * 255.0f + 0.5f);
			}
	}
}

#endif // EGFX_COLOR_FUNCTIONS_H
