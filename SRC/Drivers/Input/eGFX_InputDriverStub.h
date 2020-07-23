#include "../../eGFX.h"

#ifndef eGFX_INPUT_DRIVER_PRESENT

  #ifndef eGFX_INPUT_DRIVER_STUB_H
  #define eGFX_INPUT_DRIVER_STUB_H

  /*
      An input driver must expose a macro with the number of buttons  
  */

  #define eGFX_NUM_BUTTONS          3


  /*
      An input driver must expose an array of eGFX_Button Objects
  */

  extern eGFX_Button MyButtons[eGFX_NUM_BUTTONS];

  /*
      eGFX_InputInit() must init any related input hardware and setup the button objects
  */
  void eGFX_InputInit();


  /*
      eGFX_InputHAL_ReadPortPin() should read a physical port/pin.   The physical meaning
      of port/pin is system dependent.    They are stored with each button object and should
      translate to a port and pin on a microcontroller.
  */

  uint32_t eGFX_InputHAL_ReadPortPin(uint8_t Port, uint8_t Pin);

  #endif

#endif