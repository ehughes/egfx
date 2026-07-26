#include "egfx_config.h"
#include "egfx_types.h"
#include "egfx_pixel_ops.h"
#include "egfx_primitive_ops.h"
#include "egfx_text.h"
#include "egfx_blit.h"

//=============================================================================
// Font Helper Functions
//=============================================================================

// Get glyph for a codepoint (returns NULL if not found)
static inline const egfx_img* egfx_font_get_glyph(const egfx_font *font, uint32_t codepoint, const egfx_img *target)
{
	// Select appropriate glyph array based on target image BPP
	const egfx_img **glyphs = NULL;
	uint8_t target_bpp = target->bits_per_pixel;

	if (target_bpp <= 1 && font->glyphs_1bpp != NULL)
		glyphs = font->glyphs_1bpp;
	else if (target_bpp <= 4 && font->glyphs_4bpp != NULL)
		glyphs = font->glyphs_4bpp;
	else if (target_bpp <= 8 && font->glyphs_8bpp != NULL)
		glyphs = font->glyphs_8bpp;
	else if (font->glyphs_1bpp != NULL)  // Fallback to 1bpp if available
		glyphs = font->glyphs_1bpp;
	else if (font->glyphs_4bpp != NULL)  // Fallback to 4bpp
		glyphs = font->glyphs_4bpp;
	else if (font->glyphs_8bpp != NULL)  // Fallback to 8bpp
		glyphs = font->glyphs_8bpp;

	if (glyphs == NULL)
		return NULL;

	// For ASCII-only fonts, use direct indexing (fast path)
	if ((font->flags & EGFX_FONT_ASCII_ONLY) && codepoint >= 0x20 && codepoint < 0x80)
	{
		uint8_t index = codepoint - 0x20;
		if (index < font->glyph_count)
			return glyphs[index];
	}

	// For UTF-8 fonts with glyph map, do binary search
	if ((font->flags & EGFX_FONT_UTF8) && font->glyph_map != NULL)
	{
		// Binary search through glyph_map
		int32_t left = 0;
		int32_t right = font->glyph_map_size - 1;

		while (left <= right)
		{
			int32_t mid = (left + right) / 2;
			uint32_t map_codepoint = font->glyph_map[mid].codepoint;

			if (map_codepoint == codepoint)
			{
				uint16_t glyph_index = font->glyph_map[mid].glyph_index;
				if (glyph_index < font->glyph_count)
					return glyphs[glyph_index];
				return NULL;
			}
			else if (map_codepoint < codepoint)
			{
				left = mid + 1;
			}
			else
			{
				right = mid - 1;
			}
		}
	}

	return NULL;
}

// Get kerning adjustment for a glyph pair
// Returns 0 if no kerning found
static inline int8_t egfx_font_get_kerning(const egfx_font *font, uint16_t left_glyph_index, uint16_t right_glyph_index)
{
	if ((font->flags & EGFX_FONT_KERNING) == 0 || font->kerning_table == NULL)
		return 0;

	// Linear search through kerning table
	// TODO: Could use binary search if table is sorted
	for (uint16_t i = 0; i < font->kerning_count; i++)
	{
		const egfx_kerning_pair *pair = &font->kerning_table[i];
		if (pair->left_glyph == left_glyph_index && pair->right_glyph == right_glyph_index)
		{
			return pair->x_adjust;
		}
	}

	return 0;
}

// Decode UTF-8 sequence and return codepoint and bytes consumed
// Returns 0 if invalid sequence
static inline uint32_t egfx_utf8_decode(const char *string, uint8_t *bytes_consumed)
{
	uint8_t byte1 = (uint8_t)string[0];
	*bytes_consumed = 0;

	if (byte1 == 0)
		return 0;

	// 1-byte sequence (ASCII): 0xxxxxxx
	if ((byte1 & 0x80) == 0)
	{
		*bytes_consumed = 1;
		return byte1;
	}

	// 2-byte sequence: 110xxxxx 10xxxxxx
	if ((byte1 & 0xE0) == 0xC0)
	{
		uint8_t byte2 = (uint8_t)string[1];
		if ((byte2 & 0xC0) != 0x80)
			return 0; // Invalid continuation byte

		*bytes_consumed = 2;
		return ((byte1 & 0x1F) << 6) | (byte2 & 0x3F);
	}

	// 3-byte sequence: 1110xxxx 10xxxxxx 10xxxxxx
	if ((byte1 & 0xF0) == 0xE0)
	{
		uint8_t byte2 = (uint8_t)string[1];
		uint8_t byte3 = (uint8_t)string[2];
		if ((byte2 & 0xC0) != 0x80 || (byte3 & 0xC0) != 0x80)
			return 0; // Invalid continuation bytes

		*bytes_consumed = 3;
		return ((byte1 & 0x0F) << 12) | ((byte2 & 0x3F) << 6) | (byte3 & 0x3F);
	}

	// 4-byte sequence: 11110xxx 10xxxxxx 10xxxxxx 10xxxxxx
	if ((byte1 & 0xF8) == 0xF0)
	{
		uint8_t byte2 = (uint8_t)string[1];
		uint8_t byte3 = (uint8_t)string[2];
		uint8_t byte4 = (uint8_t)string[3];
		if ((byte2 & 0xC0) != 0x80 || (byte3 & 0xC0) != 0x80 || (byte4 & 0xC0) != 0x80)
			return 0; // Invalid continuation bytes

		*bytes_consumed = 4;
		return ((byte1 & 0x07) << 18) | ((byte2 & 0x3F) << 12) | ((byte3 & 0x3F) << 6) | (byte4 & 0x3F);
	}

	return 0; // Invalid UTF-8 sequence
}

//=============================================================================
// Text Drawing Function
//=============================================================================

void EGFX_WEAK egfx_text(egfx_img *image,
                                 char *string,
                                 egfx_point position,
                                 const egfx_text_config *config)
{
	const egfx_font *font = config->font;
	uint16_t ptr = 0;
	int32_t start_x = position.x;
	int32_t start_y = position.y;
	uint16_t prev_glyph_index = 0xFFFF; // Invalid index to start

	while ((string[ptr] != 0) && (ptr < CONFIG_EGFX_MAX_STRING_LEN))
	{
		uint8_t bytes_consumed = 0;
		uint32_t codepoint = egfx_utf8_decode(&string[ptr], &bytes_consumed);

		if (codepoint == 0 || bytes_consumed == 0)
			break; // Invalid UTF-8 or end of string

		// Calculate glyph index for kerning
		uint16_t glyph_index = 0xFFFF;
		if ((font->flags & EGFX_FONT_ASCII_ONLY) && codepoint >= 0x20 && codepoint < 0x80)
		{
			glyph_index = codepoint - 0x20;
		}
		else if ((font->flags & EGFX_FONT_UTF8) && font->glyph_map != NULL)
		{
			// Binary search to find glyph index
			int32_t left = 0;
			int32_t right = font->glyph_map_size - 1;
			while (left <= right)
			{
				int32_t mid = (left + right) / 2;
				if (font->glyph_map[mid].codepoint == codepoint)
				{
					glyph_index = font->glyph_map[mid].glyph_index;
					break;
				}
				else if (font->glyph_map[mid].codepoint < codepoint)
					left = mid + 1;
				else
					right = mid - 1;
			}
		}

		const egfx_img* glyph = egfx_font_get_glyph(font, codepoint, image);
		if (glyph != NULL)
		{
			// Apply kerning if we have a previous glyph
			if (prev_glyph_index != 0xFFFF && glyph_index != 0xFFFF)
			{
				int8_t kerning = egfx_font_get_kerning(font, prev_glyph_index, glyph_index);
				start_x += kerning;
			}

			// Don't blit space character, just advance
			if (codepoint != 0x20)
			{
				if (config->colored && config->antialiased)
				{
					// Alpha blended colored text (uses 8bpp glyphs, blends with background)
					egfx_blit_config blit_cfg = {
						.dest_img = image,
						.source_img = (egfx_img *)glyph,
						.dest_point = {start_x, start_y},
						.src_rect = {
							.p1 = {0, 0},
							.p2 = {glyph->size_x - 1, glyph->size_y - 1}
						},
						.blit_mode = EGFX_BLIT_MODE_ALPHA_BLENDED,
						.color = config->color
					};
					egfx_blit_ex(&blit_cfg);
				}
				else if (config->colored && !config->antialiased)
				{
					// Simple color replacement (uses 1bpp glyphs, no blending)
					egfx_blit_config blit_cfg = {
						.dest_img = image,
						.source_img = (egfx_img *)glyph,
						.dest_point = {start_x, start_y},
						.src_rect = {
							.p1 = {0, 0},
							.p2 = {glyph->size_x - 1, glyph->size_y - 1}
						},
						.blit_mode = EGFX_BLIT_MODE_COLORED,
						.color = config->color
					};
					egfx_blit_ex(&blit_cfg);
				}
				else
				{
					// Monochrome text - use masked blit mode
					egfx_blit_config blit_cfg = {
						.dest_img = image,
						.source_img = (egfx_img *)glyph,
						.dest_point = {start_x, start_y},
						.src_rect = {
							.p1 = {0, 0},
							.p2 = {glyph->size_x - 1, glyph->size_y - 1}
						},
						.blit_mode = EGFX_BLIT_MODE_MASKED
					};
					egfx_blit_ex(&blit_cfg);
				}
			}
			start_x += glyph->size_x;

			// Update previous glyph for next iteration
			prev_glyph_index = glyph_index;
		}

		start_x += font->inter_character_spacing;
		ptr += bytes_consumed;
	}
}
