#include "egfx_config.h"
#include "egfx_types.h"
#include "egfx_pixel_ops.h"
#include "egfx_blit.h"
#include "egfx_plane.h"
#include "egfx_color.h"

void EGFX_WEAK egfx_blit(const egfx_img *dest,
	const egfx_img *src,
	egfx_point destination_point)
{
	egfx_blit_config config = {
		.dest_img = dest,
		.source_img = src,
		.dest_point = destination_point,
		.src_rect = {
			.p1 = {0, 0},
			.p2 = {src->size_x - 1, src->size_y - 1}
		},
		.blit_mode = EGFX_BLIT_MODE_REGULAR,
		.color = {0, 0, 0}  // Not used for REGULAR mode
	};

	egfx_blit_ex(&config);
}

void EGFX_WEAK egfx_blit_ex(const egfx_blit_config *config)
{
	const egfx_img *dest = config->dest_img;
	const egfx_img *src = config->source_img;
	egfx_point dest_point = config->dest_point;
	egfx_rect src_rect = config->src_rect;

	// Determine logical dimensions for destination (transpose-aware)
	uint16_t dest_logical_size_x, dest_logical_size_y;
	if (dest->flags & EGFX_IMG_FLAGS_TRANSPOSE) {
		dest_logical_size_x = dest->size_y;
		dest_logical_size_y = dest->size_x;
	} else {
		dest_logical_size_x = dest->size_x;
		dest_logical_size_y = dest->size_y;
	}

	// Calculate destination region
	int32_t x1_dst_region = dest_point.x;
	int32_t y1_dst_region = dest_point.y;
	int32_t x2_dst_region = dest_point.x + (src_rect.p2.x - src_rect.p1.x);
	int32_t y2_dst_region = dest_point.y + (src_rect.p2.y - src_rect.p1.y);

	// Reject if completely offscreen (use logical dimensions)
	if (x1_dst_region >= dest_logical_size_x) return;
	if (x2_dst_region < 0) return;
	if (y1_dst_region >= dest_logical_size_y) return;
	if (y2_dst_region < 0) return;

	// Calculate source bounds
	int32_t src_start_x = src_rect.p1.x;
	int32_t src_start_y = src_rect.p1.y;
	int32_t src_stop_x = src_rect.p2.x;
	int32_t src_stop_y = src_rect.p2.y;

	// Clip to destination bounds (use logical dimensions)
	if (x1_dst_region < 0)
	{
		src_start_x -= x1_dst_region;
		x1_dst_region = 0;
	}

	if (x2_dst_region >= dest_logical_size_x)
	{
		src_stop_x -= (x2_dst_region - dest_logical_size_x + 1);
		x2_dst_region = dest_logical_size_x - 1;
	}

	if (y1_dst_region < 0)
	{
		src_start_y -= y1_dst_region;
		y1_dst_region = 0;
	}

	if (y2_dst_region >= dest_logical_size_y)
	{
		src_stop_y -= (y2_dst_region - dest_logical_size_y + 1);
		y2_dst_region = dest_logical_size_y - 1;
	}

	// Get pixel functions
	egfx_put_pixel_t* put_pixel = egfx_get_put_pixel_func(dest);
	egfx_get_pixel_t* get_pixel = egfx_get_get_pixel_func(src);

	// Blit based on mode
	switch (config->blit_mode)
	{
		case EGFX_BLIT_MODE_REGULAR:
		{
			int32_t dest_y = y1_dst_region;
			for (int32_t src_y = src_start_y; src_y <= src_stop_y; src_y++, dest_y++)
			{
				int32_t dest_x = x1_dst_region;
				for (int32_t src_x = src_start_x; src_x <= src_stop_x; src_x++, dest_x++)
				{
					put_pixel(dest, dest_x, dest_y, get_pixel(src, src_x, src_y));
				}
			}
			break;
		}

		case EGFX_BLIT_MODE_MASKED:
		{
			int32_t dest_y = y1_dst_region;
			for (int32_t src_y = src_start_y; src_y <= src_stop_y; src_y++, dest_y++)
			{
				int32_t dest_x = x1_dst_region;
				for (int32_t src_x = src_start_x; src_x <= src_stop_x; src_x++, dest_x++)
				{
					egfx_pixel_state pixel = get_pixel(src, src_x, src_y);
					if (pixel != 0)
					{
						put_pixel(dest, dest_x, dest_y, pixel);
					}
				}
			}
			break;
		}

		case EGFX_BLIT_MODE_COLORED:
		{
			uint8_t src_bpp = src->bits_per_pixel;
			uint16_t dest_type = EGFX_IMG_GET_TYPE(dest);

			int32_t dest_y = y1_dst_region;
			for (int32_t src_y = src_start_y; src_y <= src_stop_y; src_y++, dest_y++)
			{
				int32_t dest_x = x1_dst_region;
				for (int32_t src_x = src_start_x; src_x <= src_stop_x; src_x++, dest_x++)
				{
					egfx_pixel_state src_pixel = get_pixel(src, src_x, src_y);

					// Normalize intensity to 0-255 based on source BPP
					uint8_t intensity;
					if (src_bpp == 1)
						intensity = src_pixel ? 255 : 0;
					else if (src_bpp == 4)
						intensity = (src_pixel << 4) | src_pixel;  // 0→0, 15→255
					else if (src_bpp == 8)
						intensity = src_pixel & 0xFF;  // Mask to 8 bits
					else
						intensity = src_pixel & 0xFF;  // Default: assume 8-bit intensity

					// Skip transparent pixels
					if (intensity < 16)
						continue;

					// Apply shading to RGB color (using >> 8 for fast divide by 256)
					uint8_t r_shaded = (config->color.r * intensity) >> 8;
					uint8_t g_shaded = (config->color.g * intensity) >> 8;
					uint8_t b_shaded = (config->color.b * intensity) >> 8;

					// Convert to destination pixel format
					egfx_pixel_state dest_pixel;
					switch (dest_type)
					{
						case EGFX_IMG_16BPP_RGB565:
							dest_pixel = EGFX_COLOR_RGB888_TO_RGB565(r_shaded, g_shaded, b_shaded);
							break;

						case EGFX_IMG_32BPP_XRGB888:
							dest_pixel = EGFX_COLOR_RGB_TO_RGBX888(r_shaded, g_shaded, b_shaded);
							break;

						case EGFX_IMG_8BPP_XRGB222:
							// XRGB222: 2 bits per channel
							dest_pixel = ((r_shaded >> 6) << 4) | ((g_shaded >> 6) << 2) | (b_shaded >> 6);
							break;

						case EGFX_IMG_4BPP:
							// Grayscale: simple average
							dest_pixel = (r_shaded + g_shaded + b_shaded) / 3;
							dest_pixel >>= 4;  // Scale to 4-bit
							break;

						default:
							// Fallback: just use intensity as grayscale
							dest_pixel = intensity;
							break;
					}

					put_pixel(dest, dest_x, dest_y, dest_pixel);
				}
			}
			break;
		}

		case EGFX_BLIT_MODE_ALPHA_BLENDED:
		{
			uint8_t src_bpp = src->bits_per_pixel;
			uint16_t dest_type = EGFX_IMG_GET_TYPE(dest);

			// Convert text color to floating-point (0.0-1.0 range)
			egfx_argb_f fg_color;
			fg_color.a = 1.0f;
			fg_color.r = (float)config->color.r / 255.0f;
			fg_color.g = (float)config->color.g / 255.0f;
			fg_color.b = (float)config->color.b / 255.0f;

			int32_t dest_y = y1_dst_region;
			for (int32_t src_y = src_start_y; src_y <= src_stop_y; src_y++, dest_y++)
			{
				int32_t dest_x = x1_dst_region;
				for (int32_t src_x = src_start_x; src_x <= src_stop_x; src_x++, dest_x++)
				{
					egfx_pixel_state src_pixel = get_pixel(src, src_x, src_y);

					// Normalize intensity to 0.0-1.0 based on source BPP
					// This becomes our alpha value

                    // Skip off pixels
					if (src_pixel == 0)
						continue;

					float alpha;
					if (src_bpp == 1)
						alpha = src_pixel ? 1.0f : 0.0f;
					else if (src_bpp == 4)
						alpha = (float)(src_pixel & 0x0F) / 15.0f;
					else if (src_bpp == 8)
						alpha = (float)(src_pixel & 0xFF) / 255.0f;
					else
						alpha = (float)(src_pixel & 0xFF) / 255.0f;  // Default: assume 8-bit intensity

	

					// Read background pixel and convert to floating-point ARGB
					egfx_pixel_state bg_pixel = get_pixel(dest, dest_x, dest_y);
					egfx_argb_f bg_argb_f = egfx_pixel_to_argb_f(bg_pixel, dest_type);

					// Blend text color with background using alpha (in floating-point)
					egfx_argb_f result;
					result.a = 1.0f;  // Output is opaque
					result.r = fg_color.r * alpha + bg_argb_f.r * (1.0f - alpha);
					result.g = fg_color.g * alpha + bg_argb_f.g * (1.0f - alpha);
					result.b = fg_color.b * alpha + bg_argb_f.b * (1.0f - alpha);

					// Convert back to destination format with saturation and write
					egfx_pixel_state dest_pixel = egfx_argb_f_to_pixel(result, dest_type);
					put_pixel(dest, dest_x, dest_y, dest_pixel);
				}
			}
			break;
		}

		default:
			break;
	}
}

void EGFX_WEAK egfx_blit_faded(egfx_img *dest,
                               const egfx_img *src,
                               egfx_point position,
                               uint8_t fade)
{
	// Determine logical dimensions for destination (transpose-aware)
	uint16_t dest_logical_size_x, dest_logical_size_y;
	if (dest->flags & EGFX_IMG_FLAGS_TRANSPOSE) {
		dest_logical_size_x = dest->size_y;
		dest_logical_size_y = dest->size_x;
	} else {
		dest_logical_size_x = dest->size_x;
		dest_logical_size_y = dest->size_y;
	}

	// Calculate destination region
	int32_t x1_dst = position.x;
	int32_t y1_dst = position.y;
	int32_t x2_dst = position.x + src->size_x - 1;
	int32_t y2_dst = position.y + src->size_y - 1;

	// Reject if completely offscreen
	if (x1_dst >= dest_logical_size_x) return;
	if (x2_dst < 0) return;
	if (y1_dst >= dest_logical_size_y) return;
	if (y2_dst < 0) return;

	// Calculate source bounds
	int32_t src_start_x = 0;
	int32_t src_start_y = 0;
	int32_t src_stop_x = src->size_x - 1;
	int32_t src_stop_y = src->size_y - 1;

	// Clip to destination bounds
	if (x1_dst < 0) {
		src_start_x -= x1_dst;
		x1_dst = 0;
	}
	if (x2_dst >= dest_logical_size_x) {
		src_stop_x -= (x2_dst - dest_logical_size_x + 1);
	}
	if (y1_dst < 0) {
		src_start_y -= y1_dst;
		y1_dst = 0;
	}
	if (y2_dst >= dest_logical_size_y) {
		src_stop_y -= (y2_dst - dest_logical_size_y + 1);
	}

	// Get pixel functions
	egfx_put_pixel_t *put_pixel = egfx_get_put_pixel_func(dest);
	egfx_get_pixel_t *get_pixel = egfx_get_get_pixel_func(src);

	uint8_t src_bpp = src->bits_per_pixel;
	uint16_t dest_type = EGFX_IMG_GET_TYPE(dest);

	int32_t dest_y = y1_dst;
	for (int32_t src_y = src_start_y; src_y <= src_stop_y; src_y++, dest_y++)
	{
		int32_t dest_x = x1_dst;
		for (int32_t src_x = src_start_x; src_x <= src_stop_x; src_x++, dest_x++)
		{
			egfx_pixel_state src_pixel = get_pixel(src, src_x, src_y);
			egfx_pixel_state faded_pixel;

			// Apply fade based on bit depth
			switch (src_bpp)
			{
				case 1:
					// 1BPP: threshold - if fade < 128, turn off
					faded_pixel = (fade >= 128) ? src_pixel : 0;
					break;

				case 4:
					// 4BPP grayscale: scale 0-15 value
					faded_pixel = ((src_pixel & 0x0F) * fade) >> 8;
					break;

				case 8:
					// 8BPP grayscale: scale 0-255 value
					faded_pixel = ((src_pixel & 0xFF) * fade) >> 8;
					break;

				case 16:
				{
					// 16BPP RGB565: scale each channel
					uint16_t pixel16 = (uint16_t)src_pixel;
					uint8_t r = (pixel16 >> 11) & 0x1F;
					uint8_t g = (pixel16 >> 5) & 0x3F;
					uint8_t b = pixel16 & 0x1F;
					r = (r * fade) >> 8;
					g = (g * fade) >> 8;
					b = (b * fade) >> 8;
					faded_pixel = (r << 11) | (g << 5) | b;
					break;
				}

				case 24:
				case 32:
				{
					// 24/32BPP RGB: scale each channel
					uint8_t r = (src_pixel >> 16) & 0xFF;
					uint8_t g = (src_pixel >> 8) & 0xFF;
					uint8_t b = src_pixel & 0xFF;
					r = (r * fade) >> 8;
					g = (g * fade) >> 8;
					b = (b * fade) >> 8;
					faded_pixel = (r << 16) | (g << 8) | b;
					break;
				}

				default:
					// Fallback: just scale raw value
					faded_pixel = (src_pixel * fade) >> 8;
					break;
			}

			put_pixel(dest, dest_x, dest_y, faded_pixel);
		}
	}
}
