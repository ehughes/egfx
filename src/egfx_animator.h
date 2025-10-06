#include "egfx_types.h"

#ifdef __cplusplus
extern "C" {
#endif

#ifndef eGFX_ANIMATOR_H
#define eGFX_ANIMATOR_H

uint32_t eGFX_Animator_AddToList(egfx_animator_header * AnimatorList, egfx_animator_header *AnimatorToAdd);

uint32_t eGFX_GetAnimatorCount(egfx_animator_header * Animator, egfx_animator_header **LastAnimator);

bool eGFX_AnimatorIsComplete(egfx_animator_header * A);

bool eGFX_AnimatorsAreComplete(egfx_animator_header * AnimatorList);

void eGFX_PauseAnimator(egfx_animator_header * A);

void eGFX_StartAnimator(egfx_animator_header * A);

void eGFX_PauseAnimators(egfx_animator_header * AnimatorList);

void eGFX_StartAnimators(egfx_animator_header * AnimatorList);

uint32_t eGFX_Animator_ProcessList(egfx_animator_header * AnimatorList);

void eGFX_Init_PointAnimator(egfx_point_animator *A,
	egfx_point Start,
	egfx_point End,
	egfx_point *Current, //Point this to the thing you want to animate
	uint32_t LastFrame,      //Maximum allowed frames before forced convergence
	egfx_pointf FractionToMove, //The amount of percentage of the distance the point should move each step. 
	egfx_animator_mode Mode
);

void eGFX_Init_ScalarAnimator(egfx_scalar_animator *A,
	int32_t Start,
	int32_t End,
	int32_t *Current, //Point this to the thing you want to animate
	uint32_t LastFrame,      //Maximum allowed frames before forced convergence
	float	 FractionToMove, //The amount of percentage of the distance the point should move each step.
	egfx_animator_mode Mode
);

void eGFX_Process_PointAnimator(egfx_point_animator *A);

void eGFX_Process_ScalarAnimator(egfx_scalar_animator *A);


#endif

#ifdef __cplusplus
 }
#endif
