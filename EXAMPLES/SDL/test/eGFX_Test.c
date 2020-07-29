#include <stdlib.h>
#include <stdio.h>
#include <time.h>
#include "eGFX.h"

#include "ASSETS/Sprites/Sprites_16BPP_RGB565.h"
#include "ASSETS/Fonts/Comic_Sans_MS__24px__Bold__AntiAliasGridFit_4BPP/Comic_Sans_MS__24px__Bold__AntiAliasGridFit_4BPP.h"
#include "SDL_test_common.h"
#include "ACTIVITIES/Activity__CircleTest.h"
#include "ACTIVITIES/Activity__SpriteTest.h"

#define ACTIVITY_CIRCLE	0
#define ACTIVITY_SPRITE	1

int main(int argc, char *argv[])
{
   
	eGFX_InitDriver();


	eGFX_RegisterActivity(0, "CircleTest",
							Activity__Circle_Enter,
							Activity__Circle_Process,
							Activity__Circle_Exit);

	eGFX_RegisterActivity(1, "SpriteTest",
							Activity__Sprite_Enter,
							Activity__Sprite_Process,
							Activity__Sprite_Exit);

	eGFX_InitActivities();


	eGFX_ActivitySwitch(ACTIVITY_SPRITE, 0, &Sprite_16BPP_RGB565_bg_fade);


    while (!ProcessSDL_Events()) 
		{
			SDL_Delay(24);
		
			eGFX_ProcessActivities();
		
        }

	eGFX_DeInitDriver();

    
    return 0;
}
