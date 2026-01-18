#include "../../eGFX.h"

#ifndef EGFX_DISPLAY_DRIVER_PRESENT

#ifndef EGFX_DRIVER_STUB
#define EGFX_DRIVER_STUB

  /*
      Required symbols for any driver
  */

#define EGFX_PHYSICAL_SCREEN_SIZE_X     ((uint16_t) 128)
#define EGFX_PHYSICAL_SCREEN_SIZE_Y     ((uint16_t) 64)

#define EGFX_DISPLAY_DRIVER_IMG_TYPE   EGFX_IMG_4BPP


  /*
      egfx_init_driver() should do any hardware related init for the display and
      setup egfx_back_buffer
  */

extern void  egfx_init_driver(egfx_vsync_callback_t VS);

/*
     egfx_dump() should dump an ImagePlane to the physical screen.
*/

  extern void egfx_dump(eGFX_ImagePlane *Image);

  /*
      A driver should expose at least one back buffer that is the physical screen size and have
      a matching color space.
  */


#define EGFX_NUM_BACKBUFFERS	2

extern eGFX_ImagePlane egfx_back_buffer[EGFX_NUM_BACKBUFFERS];

  /*
   *  Integer value 0 (off) to 100 (full on)
   *
   */

  extern void egfx_set_backlight(uint8_t BacklightValue);

  #endif

#endif
