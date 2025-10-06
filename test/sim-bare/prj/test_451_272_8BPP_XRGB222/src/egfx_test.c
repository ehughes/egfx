#include <stdlib.h>
#include <stdio.h>
#include <time.h>
#include "egfx.h"
#include "sprites.h"
#include "eGFX_DisplayDriver_ImGui.h"


int main(int argc, char *argv[])
{
	eGFX_InitDriver(NULL);

  	while (!ProcessSimEvents())
  	{

		//eGFX_DrawStringColored(eGFX_BackBuffer[0], "NEOS", 32, 13, &font_5_7,0x03);
		//eGFX_DrawStringColored(eGFX_BackBuffer[0], "NEOS", 32, 30, &font_5_7, 0x0C);
		//eGFX_DrawStringColored(eGFX_BackBuffer[0], "NEOS", 32, 50, &font_5_7, 0x30);
		//eGFX_DrawStringColored(eGFX_BackBuffer[0], "NEOS", 32, 70, &font_5_7, 0x3F);
		//eGFX_DrawStringColored(eGFX_BackBuffer[0], "NEOS", 32, 80, &font_5_7, 0x01F);

		egfx_blit(eGFX_BackBuffer[0], (egfx_img *)&neos_icon, (egfx_point){0, 0});
		eGFX_DrawLine(eGFX_BackBuffer[0], 0, 0, 100, 100, 3);
		eGFX_Dump(eGFX_BackBuffer[0]);
    }

	eGFX_DeInitDriver();

    return 0;
}
