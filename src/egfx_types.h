#include "stdint.h"
#include "stdbool.h"
#include "egfx_color.h"

#ifndef EGFX_TYPES_H
#define EGFX_TYPES_H

#ifndef  NULL
  #define NULL 0
#endif

#ifndef M_PI
#define M_PI 3.14159265358979323846264338327950288
#endif

// Macro to create plane type from BPP and subtype (for preprocessor use)
#define EGFX_PLANE_TYPE_PP(bpp, subtype)   (((bpp) << 8) | (subtype))

// Macro to create plane type from BPP and subtype (for runtime use with cast)
#define EGFX_PLANE_TYPE(bpp, subtype)   ((uint16_t)EGFX_PLANE_TYPE_PP(bpp, subtype))

// Macro to get combined type value from egfx_img struct
#define EGFX_IMG_GET_TYPE(img)   EGFX_PLANE_TYPE((img)->bits_per_pixel, (img)->color_type)

// Image plane type definitions using BPP and subtype (using _PP for preprocessor compatibility)
#define EGFX_IMG_1BPP		        EGFX_PLANE_TYPE_PP(1, 0)
#define EGFX_IMG_3BPP_BGR	        EGFX_PLANE_TYPE_PP(3, 0)
#define EGFX_IMG_4BPP               EGFX_PLANE_TYPE_PP(4, 0)
#define EGFX_IMG_4BPP_XBGR          EGFX_PLANE_TYPE_PP(4, 1)
#define EGFX_IMG_8BPP               EGFX_PLANE_TYPE_PP(8, 0)
#define EGFX_IMG_8BPP_XRGB222       EGFX_PLANE_TYPE_PP(8, 1)
#define EGFX_IMG_16BPP			    EGFX_PLANE_TYPE_PP(16, 0)
#define EGFX_IMG_16BPP_RGB565       EGFX_PLANE_TYPE_PP(16, 1)
#define EGFX_IMG_16BPP_BGR565       EGFX_PLANE_TYPE_PP(16, 2)
#define EGFX_IMG_16BPP_GBRG3553     EGFX_PLANE_TYPE_PP(16, 3)
#define EGFX_IMG_16BPP_GRBG3553     EGFX_PLANE_TYPE_PP(16, 4)
#define EGFX_IMG_24BPP		        EGFX_PLANE_TYPE_PP(24, 0)
#define EGFX_IMG_24BPP_RGB888       EGFX_PLANE_TYPE_PP(24, 1)
#define EGFX_IMG_32BPP              EGFX_PLANE_TYPE_PP(32, 0)
#define EGFX_IMG_32BPP_XRGB888      EGFX_PLANE_TYPE_PP(32, 1)

// Image transformation flags (stored in rsv[0])
#define EGFX_IMG_FLAGS_TRANSPOSE   (1 << 0)  // Swap X and Y coordinates (for rotated displays)
#define EGFX_IMG_FLAGS_FLIP_X      (1 << 1)  // Mirror horizontally
#define EGFX_IMG_FLAGS_FLIP_Y      (1 << 2)  // Mirror vertically

typedef struct
{
	uint8_t   color_type;
	uint8_t   bits_per_pixel;
	uint8_t   flags;         // Transformation flags (transpose, flip_x, flip_y)
	uint8_t   rsv;           // Reserved for future use
	uint8_t*  data;
	uint16_t  size_x;        // Logical width (before transpose)
	uint16_t  size_y;        // Logical height (before transpose)
	void*     user;
} egfx_img;

// Static constructor macro for egfx_img with storage
// Creates a storage array and initializes an egfx_img struct
// Usage: EGFX_IMG_MAKE(my_image, 64, 32, EGFX_IMG_32BPP_XRGB888)
// This creates both 'name' (the egfx_img struct) and 'name##_storage' (the data array)
#define EGFX_IMG_MAKE(name, width, height, img_type) \
    uint8_t name##_storage[EGFX_CALC_IMG_STORAGE_SPACE_SIZE_BY_TYPE(width, height, img_type)]; \
    egfx_img name = { \
        .color_type = (uint8_t)((img_type) & 0xFF), \
        .bits_per_pixel = (uint8_t)(((img_type) >> 8) & 0xFF), \
        .flags = 0, \
        .rsv = 0, \
        .data = name##_storage, \
        .size_x = (width), \
        .size_y = (height), \
        .user = NULL \
    }

// Helper macro to calculate storage size from image type at compile time
#define EGFX_CALC_IMG_STORAGE_SPACE_SIZE_BY_TYPE(w, h, img_type) \
    ((((img_type) >> 8) == 1) ? EGFX_CALC_1BPP_IMG_STORAGE_SPACE_SIZE(w, h) : \
     (((img_type) >> 8) == 3) ? EGFX_CALC_3BPP_IMG_STORAGE_SPACE_SIZE(w, h) : \
     (((img_type) >> 8) == 4) ? EGFX_CALC_4BPP_IMG_STORAGE_SPACE_SIZE(w, h) : \
     (((img_type) >> 8) == 8) ? EGFX_CALC_8BPP_IMG_STORAGE_SPACE_SIZE(w, h) : \
     (((img_type) >> 8) == 16) ? EGFX_CALC_16BPP_IMG_STORAGE_SPACE_SIZE(w, h) : \
     (((img_type) >> 8) == 24) ? EGFX_CALC_24BPP_IMG_STORAGE_SPACE_SIZE(w, h) : \
     (((img_type) >> 8) == 32) ? EGFX_CALC_32BPP_IMG_STORAGE_SPACE_SIZE(w, h) : 0)


#define     EGFX_CALC_1BPP_BUFFER_ROW_BYTE_SIZE(x)		     ((x+7)>>3) //Round up to the next even byte boundary
#define     EGFX_CALC_4BPP_BUFFER_ROW_BYTE_SIZE(x)		     ((x+1)>>1) //Round up to the next even byte boundary
#define     EGFX_CALC_8BPP_BUFFER_ROW_BYTE_SIZE(x)		     (x)
#define     EGFX_CALC_16BPP_BUFFER_ROW_BYTE_SIZE(x)		 (x*2)
#define     EGFX_CALC_24BPP_BUFFER_ROW_BYTE_SIZE(x)	 	 (x*3)
#define	    EGFX_CALC_32BPP_BUFFER_ROW_BYTE_SIZE(x)	 	 (x*4)

#define     EGFX_CALC_1BPP_IMG_STORAGE_SPACE_SIZE(x,y)    (EGFX_CALC_1BPP_BUFFER_ROW_BYTE_SIZE(x)	* y)
#define     EGFX_CALC_3BPP_IMG_STORAGE_SPACE_SIZE(x,y)    (((x * y * 3))/8) //This is a special type for sharp 8 color memory LCDs.   Packing is done in the data buffer so the dump to the screen is simple
#define     EGFX_CALC_4BPP_IMG_STORAGE_SPACE_SIZE(x,y)    (EGFX_CALC_4BPP_BUFFER_ROW_BYTE_SIZE(x)	* y)
#define     EGFX_CALC_8BPP_IMG_STORAGE_SPACE_SIZE(x,y)    (EGFX_CALC_8BPP_BUFFER_ROW_BYTE_SIZE(x)	* y)
#define     EGFX_CALC_16BPP_IMG_STORAGE_SPACE_SIZE(x,y)   (EGFX_CALC_16BPP_BUFFER_ROW_BYTE_SIZE(x)	* y)
#define     EGFX_CALC_24BPP_IMG_STORAGE_SPACE_SIZE(x,y)   (EGFX_CALC_24BPP_BUFFER_ROW_BYTE_SIZE(x)	* y)
#define     EGFX_CALC_32BPP_IMG_STORAGE_SPACE_SIZE(x,y)   (EGFX_CALC_32BPP_BUFFER_ROW_BYTE_SIZE(x)	* y)

#define     EGFX_GET_BPP_FROM_IMG(ip)					 (((egfx_img *)(ip))->bits_per_pixel)

#define     EGFX_CALC_IMG_STORAGE_SPACE_SIZE_FROM_IMG(ip,x,y)   ((EGFX_GET_BPP_FROM_IMG(ip) * ip->size_x * ip->size_y )/8)

#define     EGFX_IMG_BPP_FROM_TYPE(type)  (((type)>>8)&0xFF)

typedef uint32_t egfx_pixel_state;

typedef void egfx_vsync_callback_t(egfx_img*);

typedef egfx_pixel_state egfx_get_pixel_t(const void* ,uint32_t,	uint32_t );

typedef void  egfx_put_pixel_t(const void*,	uint32_t,uint32_t,egfx_pixel_state);

typedef struct
{
    int32_t x;
    int32_t y;
} egfx_point;

typedef struct
{
    float x;
    float y;
} egfx_pointf;


typedef struct
{
	int32_t x;
	int32_t y;
} egfx_vec2;

typedef struct
{
	int32_t x;
	int32_t y;
    int32_t z;
} egfx_vec3;




typedef struct
{
    egfx_point p1;
    egfx_point p2;
} egfx_box;

typedef struct
{
	egfx_point p1;
	egfx_point p2;
} egfx_rect;

typedef struct
{
    uint32_t cnt;
	egfx_point p[];
} egfx_poly;

typedef struct
{
    uint32_t cnt;
	egfx_pointf p[];
} egfx_polyf;


#define ROUND_TO_INT16_T(x)     ((int16_t)(x + 0.5f))


// Font structure - supports ASCII, UTF-8, variable width, and kerning
typedef enum
{
    EGFX_FONT_ASCII_ONLY     = 0x01,  // Simple ASCII font (0x20-0x7F), fast path
    EGFX_FONT_UTF8           = 0x02,  // UTF-8 codepoints supported
    EGFX_FONT_KERNING        = 0x04,  // Kerning table present
    EGFX_FONT_SPARSE         = 0x08,  // Sparse glyph map (not all codepoints present)
    EGFX_FONT_VARIABLE_WIDTH = 0x10,  // Variable width font (uses glyph metrics)
} egfx_font_flags;

// Glyph metrics for variable width fonts
typedef struct
{
    int8_t  x_offset;      // X offset from cursor position
    int8_t  y_offset;      // Y offset from baseline
    uint8_t x_advance;     // How far to advance cursor after this glyph
    uint8_t padding;
} egfx_glyph_metrics;

// Glyph map entry for sparse UTF-8 fonts
typedef struct
{
    uint32_t codepoint;    // UTF-8 codepoint
    uint16_t glyph_index;  // Index into glyphs array
    uint16_t padding;
} egfx_glyph_map_entry;

// Kerning pair for kerning tables
typedef struct
{
    uint16_t left_glyph;   // Index of left glyph
    uint16_t right_glyph;  // Index of right glyph
    int8_t   x_adjust;     // Adjustment to apply
    uint8_t  padding;
} egfx_kerning_pair;

typedef struct
{
    uint16_t flags;                            // egfx_font_flags
    uint16_t glyph_count;                      // Number of glyphs

    const egfx_img **glyphs_1bpp;              // NULL if 1bpp not supported
    const egfx_img **glyphs_4bpp;              // NULL if 4bpp not supported
    const egfx_img **glyphs_8bpp;              // NULL if 8bpp not supported
    const egfx_glyph_metrics *metrics;         // NULL for fixed-width, array for variable width

    const egfx_glyph_map_entry *glyph_map;     // NULL for ASCII-only, array for sparse/UTF-8
    uint16_t glyph_map_size;                   // Size of glyph map (0 for ASCII-only)

    const egfx_kerning_pair *kerning_table;    // NULL if no kerning
    uint16_t kerning_count;                    // Number of kerning pairs (0 if none)

    // TrueType metrics (Q15.16 format)
    int32_t ascent;
    int32_t descent;
    int32_t line_spacing;

    uint8_t default_advance;                   // For fixed-width fonts or fallback
    uint8_t inter_character_spacing;
    uint8_t spaces_per_tab;
    uint8_t padding;
} egfx_font;



/***
*       ____  _     _           _
*      / __ \| |   (_)         | |
*     | |  | | |__  _  ___  ___| |_ ___
*     | |  | | '_ \| |/ _ \/ __| __/ __|
*     | |__| | |_) | |  __/ (__| |_\__ \
*      \____/|_.__/| |\___|\___|\__|___/
*                 _/ |
*                |__/
*/

typedef enum
{
	EGFX_OBJECT_SPRITE         =   0x0000,
	EGFX_OBJECT_CIRCLE		   =   0x0001,
	EGFX_OBJECT_TEXT		   =   0x0002
}egfx_object_type;

typedef enum
{
   	 EGFX_OBJECT_STATE_INACTIVE          = 0,
	 EGFX_OBJECT_STATE_ACTIVE            = 1,

}egfx_object_state;


typedef enum
{
	EGFX_OBJECT_DRAW_CMD__RESTORE = 0,
	EGFX_OBJECT_DRAW_CMD__PAINT = 1,
	EGFX_NUM_DRAW_CMDS
}egfx_object_draw_command;

typedef enum
{
	EGFX_DIRTY_RECTANGLE_NO_RESTORE = 0x0,
	EGFX_DIRTY_RECTANGLE_RESTORE_BACKGROUND_COLOR = 0x01,
	EGFX_DIRTY_RECTANGLE_RESTORE_BACKGROUND_IMAGE = 0x02

}egfx_object_dirty_rectangle_option;

typedef struct
{
		egfx_object_type	 type;
        egfx_object_state state;
		void *next; //Singley Linked list
		bool invalidated;
		egfx_object_dirty_rectangle_option dirty_rectange_restore;
		egfx_pixel_state *background_color; //Background Color to use for dirty rectangles.
										  //We use indirection so it is easier to update large groups of objects
		egfx_img **background_image;//Background image to use for dirty rectangles
										  //We use indirection so it is easier to update large groups of objects

		egfx_img **target_image;

}egfx_object_header;




typedef enum
{
	EGFX_RENDER_OPTION_BLIT_TRANSPARENT  = 0x0,
	EGFX_RENDER_OPTION_BLIT_SOLID = 0x1

}egfx_object_sprite_render_option;





/***
*       _____                 _     _         ____  _     _           _
*      / ____|               | |   (_)       / __ \| |   (_)         | |
*     | |  __ _ __ __ _ _ __ | |__  _  ___  | |  | | |__  _  ___  ___| |_ ___
*     | | |_ | '__/ _` | '_ \| '_ \| |/ __| | |  | | '_ \| |/ _ \/ __| __/ __|
*     | |__| | | | (_| | |_) | | | | | (__  | |__| | |_) | |  __/ (__| |_\__ \
*      \_____|_|  \__,_| .__/|_| |_|_|\___|  \____/|_.__/| |\___|\___|\__|___/
*                      | |                              _/ |
*                      |_|                             |__/
*/


typedef struct
{
	egfx_object_header header;

	egfx_img * sprite;
	uint32_t invalid_idx;

	egfx_rect		invalidated_area[2];
	egfx_point		position;
	egfx_pixel_state transparent_color; //Color to detect for transparent blits
	egfx_object_sprite_render_option render_option;
	bool			center_on_position;
	bool double_buffered;

}egfx_obj_sprite;



typedef struct
{
	egfx_object_header header;

	int32_t radius;
	egfx_point		position;
	egfx_pixel_state color;
	bool filled;
	bool double_buffered;
	uint32_t invalid_idx;
	int32_t invalidated_radius[2];
	egfx_point	invalidated_position[2];

}egfx_obj_circle;


typedef struct
{
	egfx_object_header header;

	egfx_img * sprite;
	egfx_rect		invalidated_area;
	egfx_point		position;
	egfx_pixel_state transparent_color; //Color to detect for transparent blits
	egfx_object_sprite_render_option render_option;
	bool			center_on_position;

}egfx_obj_text;


/***
*                     _                 _                ____  _     _           _
*         /\         (_)               | |              / __ \| |   (_)         | |
*        /  \   _ __  _ _ __ ___   __ _| |_ ___  _ __  | |  | | |__  _  ___  ___| |_ ___
*       / /\ \ | '_ \| | '_ ` _ \ / _` | __/ _ \| '__| | |  | | '_ \| |/ _ \/ __| __/ __|
*      / ____ \| | | | | | | | | | (_| | || (_) | |    | |__| | |_) | |  __/ (__| |_\__ \
*     /_/    \_\_| |_|_|_| |_| |_|\__,_|\__\___/|_|     \____/|_.__/| |\___|\___|\__|___/
*                                                                  _/ |
*                                                                 |__/
*/

typedef enum
{
	EGFX_ANIMATOR_POINT  = 0x8000,
	EGFX_ANIMATOR_SCALAR = 0x8001,
}egfx_animator_type;

typedef enum
{
	EGFX_ANIMATOR_STATE_INACTIVE = 0,
	EGFX_ANIMATOR_STATE_ACTIVE =   1,
	EGFX_ANIMATOR_STATE_COMPLETE = 2,
}egfx_animator_state;


typedef void(*egfx_animator_invalidator_t)(void *);
typedef void(*egfx_animator_complete_notifier_t)(void *);


typedef struct
{
	egfx_animator_type	 type;
	egfx_animator_state   state;
	void *next;

	void *object_to_invalidate;
	egfx_animator_invalidator_t invalidator;		//Called when animation is updated.  Will pass the object_to_invalidate as void *
	void *object_to_notify;
	egfx_animator_complete_notifier_t complete;   //Called when animation is complete.  Will pass the object_to_notify as void *

}egfx_animator_header;

typedef enum
{
	EGFX_ANIMATOR_MODE__FRACTIONAL_BISECT = 0,
	EGFX_ANIMATOR_MODE__LINEAR = 1

}egfx_animator_mode;


typedef struct
{
	egfx_animator_header header;

	egfx_animator_mode mode;
	uint32_t current_frame;
	uint32_t last_frame;

	egfx_point end;
	egfx_point start;
	egfx_point *current;

	egfx_pointf fraction_to_move;

	egfx_pointf __frac;

} egfx_point_animator;


typedef struct
{
	egfx_animator_header header;

	egfx_animator_mode mode;
	uint32_t current_frame;
	uint32_t last_frame;

	int32_t end;
	int32_t start;
	int32_t *current;

	float fraction_to_move;
	float __frac;

} egfx_scalar_animator;

#endif
