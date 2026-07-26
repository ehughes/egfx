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
	#define EGFX_WEAK                                          //__attribute__((weak))   Rethink this approach.  Some builds break when eGFX is built as library
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
 *         _        _   _       _ _
 *        / \   ___| |_(_)_   _(_) |_ _   _
 *       / _ \ / __| __| \ \ / / | __| | | |
 *      / ___ \ (__| |_| |\ V /| | |_| |_| |
 *     /_/   \_\___|\__|_| \_/ |_|\__|\__, |
 *                                    |___/
 */


#define CONFIG_EGFX_ACTIVITY_STACK_DEPTH                                                                 16
#define CONFIG_EGFX_MAX_SYSTEM_ACTIVITIES                                                                16


/***
 *      ____  __  __ ____     ___        _               _
 *     | __ )|  \/  |  _ \   / _ \ _   _| |_ _ __  _   _| |_
 *     |  _ \| |\/| | |_) | | | | | | | | __| '_ \| | | | __|
 *     | |_) | |  | |  __/  | |_| | |_| | |_| |_) | |_| | |_
 *     |____/|_|  |_|_|      \___/ \__,_|\__| .__/ \__,_|\__|
 *                                          |_|
 */

#define CONFIG_EGFX_INCLUDE_BITMAP_OUTPUT_SUPPORT  (0)


//Select only one of these!!

//#define WIN32_PRESENT
//#define CHAN_FAT_FS_PRESENT


#endif
