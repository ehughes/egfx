#ifndef _EGFX_CONFIG_H_
#define _EGFX_CONFIG_H_

/***
 *       ____
 *      / ___|___  _ __ ___
 *     | |   / _ \| '__/ _ \
 *     | |__| (_) | | |  __/
 *      \____\___/|_|  \___|
 *
 */

#ifdef _MSC_VER
	#define EGFX_WEAK
#else
	#define EGFX_WEAK                                          __attribute__((weak))
#endif

// Get configuration from Zephyr Kconfig
#ifdef CONFIG_EGFX_PHYSICAL_SCREEN_SIZE_X
    #define eGFX_PHYSICAL_SCREEN_SIZE_X CONFIG_EGFX_PHYSICAL_SCREEN_SIZE_X
#else
    #define eGFX_PHYSICAL_SCREEN_SIZE_X 320
#endif

#ifdef CONFIG_EGFX_PHYSICAL_SCREEN_SIZE_Y
    #define eGFX_PHYSICAL_SCREEN_SIZE_Y CONFIG_EGFX_PHYSICAL_SCREEN_SIZE_Y
#else
    #define eGFX_PHYSICAL_SCREEN_SIZE_Y 172
#endif

// Determine image type from Kconfig
#ifdef CONFIG_EGFX_DISPLAY_DRIVER_IMG_TYPE_1BPP
    #define eGFX_DISPLAY_DRIVER_IMG_TYPE EGFX_IMG_1BPP
#elif defined(CONFIG_EGFX_DISPLAY_DRIVER_IMG_TYPE_4BPP)
    #define eGFX_DISPLAY_DRIVER_IMG_TYPE EGFX_IMG_4BPP
#elif defined(CONFIG_EGFX_DISPLAY_DRIVER_IMG_TYPE_8BPP)
    #define eGFX_DISPLAY_DRIVER_IMG_TYPE EGFX_IMG_8BPP
#elif defined(CONFIG_EGFX_DISPLAY_DRIVER_IMG_TYPE_16BPP_RGB565)
    #define eGFX_DISPLAY_DRIVER_IMG_TYPE EGFX_IMG_16BPP_RGB565
#elif defined(CONFIG_EGFX_DISPLAY_DRIVER_IMG_TYPE_32BPP_XRGB888)
    #define eGFX_DISPLAY_DRIVER_IMG_TYPE EGFX_IMG_32BPP_XRGB888
#else
    #define eGFX_DISPLAY_DRIVER_IMG_TYPE EGFX_IMG_16BPP_RGB565
#endif

#ifdef CONFIG_EGFX_NUM_BACKBUFFERS
    #define eGFX_NUM_BACKBUFFERS CONFIG_EGFX_NUM_BACKBUFFERS
#else
    #define eGFX_NUM_BACKBUFFERS 1
#endif

#define CONFIG_EGFX_PUT_PIXEL_FLIP_X                                     (0)
#define CONFIG_EGFX_PUT_PIXEL_FLIP_Y                                     (0)

/*
        0 to allocate as static
        1 to allocate on stack
*/
#define CONFIG_EGFX_STRING_BUF_ALLOC                                     (0)
#define CONFIG_EGFX_MAX_PRINTF_BUF_LENGTH                                (64)
#define CONFIG_EGFX_SPACES_PER_TAB                                       (4)
#define CONFIG_EGFX_MAX_STRING_LEN                                       (64)

/***
 *      ____  __  __ ____     ___        _               _
 *     | __ )|  \/  |  _ \   / _ \ _   _| |_ _ __  _   _| |_
 *     |  _ \| |\/| | |_) | | | | | | | | __| '_ \| | | | __|
 *     | |_) | |  | |  __/  | |_| | |_| | |_| |_) | |_| | |_
 *     |____/|_|  |_|_|      \___/ \__,_|\__| .__/ \__,_|\__|
 *                                          |_|
 */

#define CONFIG_EGFX_INCLUDE_BITMAP_OUTPUT_SUPPORT  (0)

// Note: Using Zephyr display driver API directly, not eGFX display driver

#endif
