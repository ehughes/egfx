#include <stdlib.h>
#include <stdio.h>
#include <time.h>
#include "egfx.h"
#include "sprites.h"
#include "eGFX_DisplayDriver_ImGui.h"


int main(int argc, char *argv[])
{
	
	egfx_init_driver(NULL);

	uint32_t* raw_image_array;
	raw_image_array = (uint32_t*)(egfx_back_buffer[0]->data);

  	while (!ProcessSimEvents())
  	{
		//SDL_Delay(24);

		//eGFX_DrawStringColored(egfx_back_buffer[0], "NEOS", 32, 13, &font_5_7,0x9F1f000);

		egfx_blit(egfx_back_buffer[0], (egfx_img *)&neos_icon, (egfx_point){0, 0});

		//egfx_draw_line(egfx_back_buffer[0], 0, 0, 32, 32, 255);

		egfx_dump(egfx_back_buffer[0]);
    }

	egfx_deinit_driver();
	
    return 0;
}
