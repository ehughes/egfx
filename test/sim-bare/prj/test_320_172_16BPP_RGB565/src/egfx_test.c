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
	egfx_init_driver(NULL);

	while (!ProcessSimEvents())
	{
        egfx_blit(egfx_back_buffer[0],
         &bg0,
         (egfx_point){.x=0,.y=0});
		// Title
		egfx_text(egfx_back_buffer[0], "Font Comparison Test", (egfx_point){5, 5},
			&(egfx_text_config){.font = &font_5_7, .colored = true, .color = {255, 0, 0}, .antialiased = false});

		// 14px 1BPP - Simple color replacement (sharp edges)
		egfx_text(egfx_back_buffer[0], "14px 1BPP Red", (egfx_point){5, 20},
			&(egfx_text_config){.font = &font_fragment_mono_14px_1bpp, .colored = true, .color = {255, 0, 0}, .antialiased = false});

		// 14px 8BPP - Alpha blended (smooth edges)
		egfx_text(egfx_back_buffer[0], "14px 8BPP Red", (egfx_point){5, 40},
			&(egfx_text_config){.font = &font_fragment_mono_14px_8bpp, .colored = true, .color = {255, 0, 0}, .antialiased = true});

		// 28px 1BPP - Simple color replacement (sharp edges)
		egfx_text(egfx_back_buffer[0], "28px 1BPP Blue", (egfx_point){5, 65},
			&(egfx_text_config){.font = &font_fragment_mono_28px_1bpp, .colored = true, .color = {0, 0, 255}, .antialiased = false});

		// 28px 8BPP - Alpha blended (smooth edges)
		egfx_text(egfx_back_buffer[0], "28px 8BPP Blue", (egfx_point){5, 105},
			&(egfx_text_config){.font = &font_fragment_mono_28px_8bpp, .colored = true, .color = {0, 0, 255}, .antialiased = true});

		// Orbitron with antialiasing
		egfx_text(egfx_back_buffer[0], "Orbitron WAVE Test", (egfx_point){5, 145},
			&(egfx_text_config){.font = &font_orbitron_16px_8bpp, .colored = true, .color = {0, 128, 0}, .antialiased = true});

		egfx_dump(egfx_back_buffer[0]);
	}

	egfx_deinit_driver();

	return 0;
}
