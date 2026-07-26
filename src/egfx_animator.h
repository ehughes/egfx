#include "egfx_types.h"

#ifdef __cplusplus
extern "C" {
#endif

#ifndef EGFX_ANIMATOR_H
#define EGFX_ANIMATOR_H

uint32_t egfx_animator_add_to_list(egfx_animator_header *animator_list, egfx_animator_header *animator_to_add);

uint32_t egfx_get_animator_count(egfx_animator_header *animator, egfx_animator_header **last_animator);

bool egfx_animator_is_complete(egfx_animator_header *a);

bool egfx_animators_are_complete(egfx_animator_header *animator_list);

void egfx_pause_animator(egfx_animator_header *a);

void egfx_start_animator(egfx_animator_header *a);

void egfx_pause_animators(egfx_animator_header *animator_list);

void egfx_start_animators(egfx_animator_header *animator_list);

uint32_t egfx_animator_process_list(egfx_animator_header *animator_list);

void egfx_init_point_animator(egfx_point_animator *a,
	egfx_point start,
	egfx_point end,
	egfx_point *current, //Point this to the thing you want to animate
	uint32_t last_frame,      //Maximum allowed frames before forced convergence
	egfx_pointf fraction_to_move, //The amount of percentage of the distance the point should move each step.
	egfx_animator_mode mode
);

void egfx_init_scalar_animator(egfx_scalar_animator *a,
	int32_t start,
	int32_t end,
	int32_t *current, //Point this to the thing you want to animate
	uint32_t last_frame,      //Maximum allowed frames before forced convergence
	float fraction_to_move, //The amount of percentage of the distance the point should move each step.
	egfx_animator_mode mode
);

void egfx_process_point_animator(egfx_point_animator *a);

void egfx_process_scalar_animator(egfx_scalar_animator *a);


#endif

#ifdef __cplusplus
 }
#endif
