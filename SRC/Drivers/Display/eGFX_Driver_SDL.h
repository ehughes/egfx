#include "../../eGFX.h"


#ifndef eGFX_DISPLAY_DRIVER_PRESENT

  #ifndef eGFX_DRIVER_SDL
  #define eGFX_DRIVER_SDL

 #define eGFX_DISPLAY_DRIVER_PRESENT

 #define SDL_COLOR eGFX_IMAGE_PLANE_16BPP_RGB565


  /*
      Required symbols for any driver
  */

  #define eGFX_PHYSICAL_SCREEN_SIZE_X     ((uint16_t) 480)
  #define eGFX_PHYSICAL_SCREEN_SIZE_Y     ((uint16_t) 480)

  /*
      eGFX_InitDriver() should do any hardware related init for the display and
      setup eGFX_BackBuffer
  */

  extern void  eGFX_InitDriver();

  /*
      eGFX_WaitForV_Sync() should block until the the last dump operation is complete.
      Applications that dont require this functionality should immediately return.  This
      function is useful for applications where eGFX_Dump is asynchronous.
  */

  extern void eGFX_WaitForV_Sync();

  /*
      eGFX_Dump() should dump an ImagePlane to the physical screen.
  */

  extern void eGFX_Dump(eGFX_ImagePlane *Image);

  /*
      A driver should expose at least one back buffer that is the physical screen size and have
      a matching color space.
  */
	
 #define eGFX_NUM_BACKBUFFERS 2

  extern eGFX_ImagePlane eGFX_BackBuffer[eGFX_NUM_BACKBUFFERS];

  /*
   *  Integer value 0 (off) to 100 (full on)
   *
   */

  extern void eGFX_SetBacklight(uint8_t BacklightValue);


  /*
  
  Some SDL Specific stuff to make the example work
  
  */
  int ProcessSDL_Events();
  void eGFX_SDL_Quit();

  #endif

#endif
