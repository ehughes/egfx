#include "eGFX_DataTypes.h"

#ifdef __cplusplus
extern "C" {
#endif

#ifndef eGFX_ANIMATOR_H
#define eGFX_ANIMATOR_H

uint32_t eGFX_Animator_AddToList(eGFX_AnimatorHeader * AnimatorList, eGFX_AnimatorHeader *AnimatorToAdd);

uint32_t eGFX_GetAnimatorCount(eGFX_AnimatorHeader * Animator, eGFX_AnimatorHeader **LastAnimator);

bool eGFX_AnimatorIsComplete(eGFX_AnimatorHeader * A);

bool eGFX_AnimatorsAreComplete(eGFX_AnimatorHeader * AnimatorList);

void eGFX_PauseAnimator(eGFX_AnimatorHeader * A);

void eGFX_StartAnimator(eGFX_AnimatorHeader * A);

void eGFX_PauseAnimators(eGFX_AnimatorHeader * AnimatorList);

void eGFX_StartAnimators(eGFX_AnimatorHeader * AnimatorList);

uint32_t eGFX_Animator_ProcessList(eGFX_AnimatorHeader * AnimatorList);

void eGFX_Init_PointAnimator(eGFX_Point_Animator *A,
	eGFX_Point Start,
	eGFX_Point End,
	eGFX_Point *Current, //Point this to the thing you want to animate
	uint32_t LastFrame,      //Maximum allowed frames before forced convergence
	eGFX_PointF FractionToMove, //The amount of percentage of the distance the point should move each step. 
	eGFX_AnimatorMode Mode
);

void eGFX_Init_ScalarAnimator(eGFX_Scalar_Animator *A,
	int32_t Start,
	int32_t End,
	int32_t *Current, //Point this to the thing you want to animate
	uint32_t LastFrame,      //Maximum allowed frames before forced convergence
	float	 FractionToMove, //The amount of percentage of the distance the point should move each step.
	eGFX_AnimatorMode Mode
);

void eGFX_Process_PointAnimator(eGFX_Point_Animator *A);

void eGFX_Process_ScalarAnimator(eGFX_Scalar_Animator *A);


#endif

#ifdef __cplusplus
 }
#endif
