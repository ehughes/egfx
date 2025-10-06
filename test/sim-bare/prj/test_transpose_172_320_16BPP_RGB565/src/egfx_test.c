#include <stdlib.h>
#include <stdio.h>
#include <time.h>
#include "egfx.h"
#include "sprites.h"
#include "eGFX_DisplayDriver_ImGui.h"
#include "font_fragment_mono_14px_1bpp.h"
#include "font_fragment_mono_14px_8bpp.h"
#include "font_fragment_mono_28px_1bpp.h"
#include "font_fragment_mono_28px_8bpp.h"
#include "font_orbitron_16px_8bpp.h"


int main(int argc, char *argv[])
{
	eGFX_InitDriver(NULL);

	// Reconfigure backbuffers for transpose mode
	// Physical memory layout: 172x320 pixels (created by driver)
	// Logical dimensions: 320x172 (what we use in code with transpose flag)

	// Swap the dimensions and set transpose flag for both backbuffers
	for (int i = 0; i < eGFX_NUM_BACKBUFFERS; i++) {
		uint16_t temp = eGFX_BackBuffer[i]->size_x;
	
		eGFX_BackBuffer[i]->flags = EGFX_IMG_FLAGS_TRANSPOSE ;
	}

	while (!ProcessSimEvents())
	{
		// Clear screen to white
		// We use logical coordinates: 320x172fdsfd

        egfx_blit(eGFX_BackBuffer[0],
         &bg0,
         (egfx_point){.x=0,.y=0});

		// Title - positioned using logical 320x172 coordinates
		egfx_text(eGFX_BackBuffer[0], "Transpose Test (172x320 physical -> 320x172 logical)", (egfx_point){5, 5},
			&(egfx_text_config){.font = &font_5_7, .colored = true, .color = {255, 0, 0}, .antialiased = false});

		// 14px 1BPP - Simple color replacement (sharp edges)
		egfx_text(eGFX_BackBuffer[0], "14px 1BPP Red", (egfx_point){5, 20},
			&(egfx_text_config){.font = &font_fragment_mono_14px_1bpp, .colored = true, .color = {255, 0, 0}, .antialiased = false});

            	eGFX_Dump(eGFX_BackBuffer[0]);
	}

	eGFX_DeInitDriver();

	return 0;
}
