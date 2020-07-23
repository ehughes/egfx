#include "../../eGFX.h"
#include "eGFX_Driver_SDL.h"

#ifdef eGFX_DRIVER_SDL

#include "SDL_test_common.h"

static SDLTest_CommonState *state;

long ScaleFactor = 1;
int KeyCode = 0;

int ProcessSDL_Events()
{

	int done = 0;

	int i;
	SDL_Event event;
	   	 
	/* Check for events */
	while (SDL_PollEvent(&event))
	{
		SDLTest_CommonEvent(state, &event, &done);


		switch (event.type)
		{

		default:
			
			break;

		case SDL_KEYDOWN:

	
			KeyCode = event.key.keysym.scancode;

			break;


		case SDL_KEYUP:


			KeyCode = event.key.keysym.scancode;

			break;
			/* Mouse events */
		case	SDL_MOUSEMOTION: /**< Mouse moved */
		case		SDL_MOUSEBUTTONDOWN:       /**< Mouse button pressed */
		case		SDL_MOUSEBUTTONUP:          /**< Mouse button released */
		case		SDL_MOUSEWHEEL:             /**< Mouse wheel motion */
			break;


			case SDL_WINDOWEVENT:
				switch (event.window.event)
				{
				

					case SDL_WINDOWEVENT_RESIZED:
					//case SDL_WINDOWEVENT_SIZE_CHANGED:
					{
						SDL_Window *window = SDL_GetWindowFromID(event.window.windowID);
						if (window)
						{
							for (i = 0; i < state->num_windows; ++i)
							{
								if (window == state->windows[i])
								{
									//ScaleFactor = 
										//SDL_SetWindowSize(state->windows[i]->

									int w, h;

									SDL_GetWindowSize(state->windows[i], &w, &h);

									ScaleFactor = (w / eGFX_PHYSICAL_SCREEN_SIZE_X) + 1;

									w = eGFX_PHYSICAL_SCREEN_SIZE_X * ScaleFactor;
									h = eGFX_PHYSICAL_SCREEN_SIZE_Y * ScaleFactor;

									SDL_SetWindowSize(state->windows[i], w, h);


								}
							}
						}
					}
					break;
				}
				break;

		}
	}

	return done;
}

void eGFX_SDL_Quit()
{
	SDLTest_CommonQuit(state);
}

eGFX_ImagePlane eGFX_BackBuffer[eGFX_NUM_BACKBUFFERS];


#if (SDL_COLOR == eGFX_IMAGE_PLANE_1BPP)

uint8_t eGFX_FrameBuffer[eGFX_NUM_BACKBUFFERS][eGFX_CALCULATE_1BPP_IMAGE_STORAGE_SPACE_SIZE(eGFX_PHYSICAL_SCREEN_SIZE_X, eGFX_PHYSICAL_SCREEN_SIZE_Y)];

#elif SDL_COLOR == eGFX_IMAGE_PLANE_3BPP_BGR

uint8_t eGFX_FrameBuffer[eGFX_CALCULATE_3BPP_IMAGE_STORAGE_SPACE_SIZE(eGFX_PHYSICAL_SCREEN_SIZE_X, eGFX_PHYSICAL_SCREEN_SIZE_Y)];

#elif SDL_COLOR == eGFX_IMAGE_PLANE_4BPP

uint8_t eGFX_FrameBuffer[eGFX_NUM_BACKBUFFERS][eGFX_CALCULATE_4BPP_IMAGE_STORAGE_SPACE_SIZE(eGFX_PHYSICAL_SCREEN_SIZE_X, eGFX_PHYSICAL_SCREEN_SIZE_Y)];

#elif SDL_COLOR == eGFX_IMAGE_PLANE_8BPP

uint8_t eGFX_FrameBuffer[eGFX_NUM_BACKBUFFERS][eGFX_CALCULATE_8BPP_IMAGE_STORAGE_SPACE_SIZE(eGFX_PHYSICAL_SCREEN_SIZE_X, eGFX_PHYSICAL_SCREEN_SIZE_Y)];

#elif SDL_COLOR == eGFX_IMAGE_PLANE_16BPP_RGB565

uint8_t eGFX_FrameBuffer[eGFX_NUM_BACKBUFFERS][eGFX_CALCULATE_16BPP_IMAGE_STORAGE_SPACE_SIZE(eGFX_PHYSICAL_SCREEN_SIZE_X, eGFX_PHYSICAL_SCREEN_SIZE_Y)];

#elif SDL_COLOR == eGFX_IMAGE_PLANE_16BPP_BGR565

uint8_t eGFX_FrameBuffer[eGFX_NUM_BACKBUFFERS][eGFX_CALCULATE_16BPP_IMAGE_STORAGE_SPACE_SIZE(eGFX_PHYSICAL_SCREEN_SIZE_X, eGFX_PHYSICAL_SCREEN_SIZE_Y)];

#elif SDL_COLOR == eGFX_IMAGE_PLANE_16BPP_GBRG3553

uint8_t eGFX_FrameBuffer[eGFX_NUM_BACKBUFFERS][eGFX_CALCULATE_16BPP_IMAGE_STORAGE_SPACE_SIZE(eGFX_PHYSICAL_SCREEN_SIZE_X, eGFX_PHYSICAL_SCREEN_SIZE_Y)];

#elif SDL_COLOR == eGFX_IMAGE_PLANE_16BPP_GRBG3553

uint8_t eGFX_FrameBuffer[eGFX_NUM_BACKBUFFERS][eGFX_CALCULATE_16BPP_IMAGE_STORAGE_SPACE_SIZE(eGFX_PHYSICAL_SCREEN_SIZE_X, eGFX_PHYSICAL_SCREEN_SIZE_Y)];

#elif SDL_COLOR == eGFX_IMAGE_PLANE_24BPP

uint8_t eGFX_FrameBuffer[eGFX_NUM_BACKBUFFERS][eGFX_CALCULATE_24BPP_IMAGE_STORAGE_SPACE_SIZE(eGFX_PHYSICAL_SCREEN_SIZE_X, eGFX_PHYSICAL_SCREEN_SIZE_Y)];

#elif SDL_COLOR == eGFX_IMAGE_PLANE_32BPP

uint8_t eGFX_FrameBuffer[eGFX_NUM_BACKBUFFERS][eGFX_CALCULATE_32BPP_IMAGE_STORAGE_SPACE_SIZE(eGFX_PHYSICAL_SCREEN_SIZE_X, eGFX_PHYSICAL_SCREEN_SIZE_Y)];

#else 

#error "I need a backbuffer image plane type"

#endif


char * Dummy[4] = { "eGFX Test : SDL Driver","","",(char *)0};


SDL_Texture *sdlTexture;

void eGFX_InitDriver()
{

	for (int i = 0; i < eGFX_NUM_BACKBUFFERS; i++)
	{

		eGFX_ImagePlaneInit(&eGFX_BackBuffer[i],
			&eGFX_FrameBuffer[i][0],
			eGFX_PHYSICAL_SCREEN_SIZE_X,
			eGFX_PHYSICAL_SCREEN_SIZE_Y,
			eGFX_IMAGE_PLANE_16BPP_RGB565);

		memset(eGFX_FrameBuffer, 0, sizeof(eGFX_FrameBuffer));
	}



	Uint32 then, now, frames;
	int i;

	/* Enable standard application logging */
	SDL_LogSetPriority(SDL_LOG_CATEGORY_APPLICATION, SDL_LOG_PRIORITY_INFO);

	/* Initialize test framework */
	state = SDLTest_CommonCreateState(Dummy, SDL_INIT_VIDEO);
	
	state->window_w = eGFX_PHYSICAL_SCREEN_SIZE_X;
	state->window_h = eGFX_PHYSICAL_SCREEN_SIZE_Y;
	state->window_flags = SDL_WINDOW_RESIZABLE;

	if (!state) {
		return ;
	}
	if (!SDLTest_CommonInit(state)) {
		return ;
	}
	
	/* Create the windows and initialize the renderers */
	for (i = 0; i < state->num_windows; ++i) {
		SDL_Renderer *renderer = state->renderers[i];
		SDL_SetRenderDrawBlendMode(renderer, SDL_BLENDMODE_NONE);
		SDL_SetRenderDrawColor(renderer, 0x10, 0x10, 0x12, 0xFF);
		SDL_RenderClear(renderer);
	}
	
		sdlTexture = SDL_CreateTexture(state->renderers[0],
			SDL_PIXELFORMAT_ARGB8888,
			SDL_TEXTUREACCESS_STREAMING,
			eGFX_PHYSICAL_SCREEN_SIZE_X, eGFX_PHYSICAL_SCREEN_SIZE_Y);

}

uint32_t TextureBuffer[eGFX_PHYSICAL_SCREEN_SIZE_X*eGFX_PHYSICAL_SCREEN_SIZE_Y];


void eGFX_WaitForV_Sync()
{
    
}


void eGFX_Dump(eGFX_ImagePlane *Image)
{
	eGFX_PixelState PS;
	int r, g, b;
	SDL_Rect rect;
	rect.w = ScaleFactor;
	rect.h = ScaleFactor;

	for (int i = 0; i < state->num_windows; ++i)
	{
		SDL_Renderer *renderer = state->renderers[i];
		if (state->windows[i] == NULL)
			continue;

		SDL_RenderClear(renderer);

		SDL_RenderCopy(renderer, sdlTexture, NULL, NULL);

			for (int y = 0; y < eGFX_PHYSICAL_SCREEN_SIZE_Y; y++)
			{
				for (int x = 0; x < eGFX_PHYSICAL_SCREEN_SIZE_X; x++)
				{
					PS = eGFX_GetPixel(Image, x, y);

					r = 0;
					g = 0;
					b = 0;

					switch (Image->Type)
					{
					case eGFX_IMAGE_PLANE_1BPP:

						if (PS)
						{
							r = 0xFF;
							g = 0xFF;
							b = 0xFF;
						}

						break;

					case eGFX_IMAGE_PLANE_4BPP:

						r = PS;
						g = PS * 12;
						b = PS << 1;


						break;


					case eGFX_IMAGE_PLANE_8BPP:

						if (PS)
						{
							r = PS;
							g = PS;
							b = PS;
						}


						break;

					case eGFX_IMAGE_PLANE_16BPP_RGB565:
						if (PS)
						{
							r = (PS>>11) & (0x1F);
							g = (PS>>5) & (0x3F);
							b = (PS) & (0x1F);
						}

						break;

					case eGFX_IMAGE_PLANE_16BPP_BGR565:
						if (PS)
						{
							r = (PS >> 11) & (0x1F);
							g = (PS >> 5) & (0x3F);
							b = (PS) & (0x1F);
						}

						break;
					}

					TextureBuffer[y*eGFX_PHYSICAL_SCREEN_SIZE_X + x] = (r << 16+3) + (g << 8+2) + (b<<3);

				}
			}
		
			SDL_UpdateTexture(sdlTexture, NULL, &TextureBuffer, eGFX_PHYSICAL_SCREEN_SIZE_X * sizeof(Uint32));
		
		
		SDL_RenderPresent(renderer);


	}

}

void eGFX_SetBacklight(uint8_t BacklightValue)
{



}




#endif
