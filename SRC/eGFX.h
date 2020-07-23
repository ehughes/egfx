
#ifdef __cplusplus
extern "C" {
#endif

#ifndef _eGFX_H__
#define _eGFX_H__

#define eGFX_API_LEVEL                                                                                     1
#define eGFX_REVISION                                                                                      2

#include "eGFX_Config.h"

#include <stdint.h>
#include "Core/eGFX_DataTypes.h"
#include "Core/eGFX_Plane.h"


#include "Core/eGFX_Blit.h"
#include "Core/eGFX_Obj.h"
#include "Core/eGFX_Obj__Circle.h"
#include "Core/eGFX_Obj__Sprite.h"

#include "Core/eGFX_PixelOps.h"

#include "Core/eGFX_PrimitiveOps.h"
#include "Core/eGFX_StringOps.h"
#include "Core/eGFX_Animator.h"

#include "Core/Fonts/FONT_3_5_1BPP/FONT_3_5_1BPP.h"
#include "Core/Fonts/FONT_5_7_1BPP/FONT_5_7_1BPP.h"
#include "Core/Fonts/FONT_10_14_1BPP/FONT_10_14_1BPP.h"

#include "Input/eGFX_InputTypes.h"
#include "Input/eGFX_Input.h"

#include "UI/eGFX_Activity.h"

#include "eGFX_Driver.h"


#ifndef eGFX_DISPLAY_DRIVER_PRESENT

    #include "Drivers/Display/eGFX_Driver_DisplayStub.h"
	
	#ifdef _MSC_VER
		
	#else
		#warning "eGFX display driver not selected.  Using stub."
	#endif

#endif

#ifndef eGFX_INPUT_DRIVER_PRESENT

    #include "Drivers/Input/eGFX_InputDriverStub.h"

	#ifdef _MSC_VER

	#else
				#warning "eGFX input driver not selected.  Using stub."
	#endif

#endif


#endif

#ifdef __cplusplus
 }
#endif

