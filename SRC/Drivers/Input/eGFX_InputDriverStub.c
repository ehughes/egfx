#include "eGFX_InputDriverStub.h"

#ifndef eGFX_INPUT_DRIVER_PRESENT

eGFX_Button MyButtons[eGFX_NUM_BUTTONS];

void eGFX_InputInit()
{
            eGFX_InitButton(&MyButtons[0],
                        0,
                        1,
                        eGFX_BUTTON_POLARITY_LOW_ACTIVE,
                        1,
                        "LEFT");

            eGFX_InitButton(&MyButtons[1],
                        0,
                        2,
                        eGFX_BUTTON_POLARITY_LOW_ACTIVE,
                        1,
                        "MIDDLE");

            eGFX_InitButton(&MyButtons[2],
                        0,
                        2,
                        eGFX_BUTTON_POLARITY_LOW_ACTIVE,
                        1,
                        "RIGHT");

}

uint32_t eGFX_InputHAL_ReadPortPin(uint8_t Port, uint8_t Pin)
{
    return 0;
}


#endif