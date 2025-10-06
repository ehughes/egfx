#include <stdlib.h>
#include <stdio.h>
#include <time.h>
#include "egfx.h"
#include "sprites.h"
#include "eGFX_DisplayDriver_ImGui.h"


int main(int argc, char *argv[])
{
	
	eGFX_InitDriver(NULL);

	uint32_t* raw_image_array;
	raw_image_array = (uint32_t*)(eGFX_BackBuffer[0]->data);

  	while (!ProcessSimEvents())
  	{
		//SDL_Delay(24);

		//eGFX_DrawStringColored(eGFX_BackBuffer[0], "NEOS", 32, 13, &font_5_7,0x9F1f000);

		egfx_blit(eGFX_BackBuffer[0], (egfx_img *)&neos_icon, (egfx_point){0, 0});

		//eGFX_DrawLine(eGFX_BackBuffer[0], 0, 0, 32, 32, 255);

		eGFX_Dump(eGFX_BackBuffer[0]);
    }

	eGFX_DeInitDriver();
	
    return 0;
}
