
#include "egfx_animator.h"
#include "math.h"

bool egfx_animators_are_complete(egfx_animator_header *animator_list)
{
	bool ret_val = true;

	while (animator_list != NULL)
	{
		if (egfx_animator_is_complete(animator_list) == false)
		{
			ret_val = false;
			break;
		}

		animator_list = ((egfx_animator_header *)(animator_list))->next;
	}

	return ret_val;
}


bool egfx_animator_is_complete(egfx_animator_header *a)
{
	bool ret_val = false;

	if (((egfx_animator_header *)(a))->state == EGFX_ANIMATOR_STATE_COMPLETE)
	{
		ret_val = true;
	}

	return ret_val;
}

void egfx_pause_animator(egfx_animator_header *a)
{
	((egfx_animator_header *)(a))->state = EGFX_OBJECT_STATE_INACTIVE;
}

void egfx_start_animator(egfx_animator_header *a)
{
	(*((egfx_animator_header *)(a))).state = EGFX_OBJECT_STATE_ACTIVE;
}


void egfx_pause_animators(egfx_animator_header *animator_list)
{
	while (animator_list != NULL)
	{
		egfx_pause_animator(animator_list);
		animator_list = ((egfx_animator_header *)(animator_list))->next;
	}
}

void egfx_start_animators(egfx_animator_header *animator_list)
{
	while (animator_list != NULL)
	{
		egfx_start_animator(animator_list);
		animator_list = ((egfx_animator_header *)(animator_list))->next;
	}
}


static void egfx_animator_mark_complete(egfx_animator_header *a)
{
	a->state = EGFX_ANIMATOR_STATE_COMPLETE;

	if (a->complete != NULL)
	{
		a->complete(a->object_to_notify);
	}
}


static void egfx_invalidate_animator_object(egfx_animator_header *a)
{
	if (a != NULL)
	{
		if (a->invalidator != NULL)
		{
			a->invalidator(a->object_to_invalidate);
		}
	}
}

static void egfx_invalidate_all_animator_objects(egfx_animator_header *animator_list)
{
	while (animator_list != NULL)
	{
		egfx_invalidate_animator_object(animator_list);
		animator_list = ((egfx_animator_header *)(animator_list))->next;
	}
}

uint32_t egfx_animator_add_to_list(egfx_animator_header *animator_list, egfx_animator_header *animator_to_add)
{
	egfx_animator_header *last_object = NULL;
	uint32_t num_objects = 0;

	if (animator_list != NULL)
	{
		num_objects = egfx_get_animator_count(animator_list, &last_object);

		if ((last_object != NULL) && (animator_to_add != NULL))
		{
			((egfx_animator_header *)last_object)->next = animator_to_add;

			((egfx_animator_header *)animator_to_add)->next = NULL;
		}
	}

	return num_objects;
}

uint32_t egfx_get_animator_count(egfx_animator_header *animator, egfx_animator_header **last_animator)
{
	uint32_t count = 0;

	if (animator != NULL)
	{
		count = 1;
		while (
			(((egfx_object_header *)animator)->next != NULL)
			)
		{
			animator = ((egfx_object_header *)animator)->next;
			count++;
		}

		*last_animator = animator;
	}

	return count;
}


bool egfx_animator_process(egfx_animator_header *a)
{
	bool processed = true;

	if ((egfx_object_state)((egfx_animator_header *)(a))->state == (egfx_object_state)EGFX_OBJECT_STATE_ACTIVE)
	{
		switch (((egfx_animator_header *)(a))->type)
		{

		case EGFX_ANIMATOR_POINT:
			egfx_process_point_animator((egfx_point_animator *)a);
			break;

		case EGFX_ANIMATOR_SCALAR:
			egfx_process_scalar_animator((egfx_scalar_animator *)a);
			break;

		default:
			processed = false;
			break;
		}
	}
	else
	{
		processed = false;
	}

	return processed;
}

uint32_t egfx_animator_process_list(egfx_animator_header *animator_list)
{
	uint32_t processed_animators = 0;
	egfx_animator_header *animator_start = animator_list;

	//Process the list
	while (animator_list != NULL)
	{
		if (egfx_animator_process(animator_list))
		{
			processed_animators++;
		}
		animator_list = ((egfx_object_header *)(animator_list))->next;
	}

	//We need to keep invalidating all animator objects until they are all done to avoid visual artifacts
	animator_list = animator_start;
	if (egfx_animators_are_complete(animator_list) == false)
	{
		egfx_invalidate_all_animator_objects(animator_list);
	}

	return processed_animators;
}

//Initializes the data struct
void egfx_init_point_animator(egfx_point_animator *a,
							egfx_point start,
							egfx_point end,
							egfx_point *current, //Point this to the thing you want to animate
							uint32_t last_frame,      //Maximum allowed frames before forced convergence
							egfx_pointf fraction_to_move, //The amount of percentage of the distance the point should move each step. Values <=0 or >= 1 are set to 0.5
							egfx_animator_mode mode
						)
{
	if (a != NULL)
	{
		a->header.type = EGFX_ANIMATOR_POINT;
		a->header.state = EGFX_ANIMATOR_STATE_INACTIVE;
		a->start = start;
		a->end = end;

		a->__frac.x = (float)a->start.x;
		a->__frac.y = (float)a->start.y;
		a->current = current;

		if (current != NULL)
		{
			*(a->current) = a->start;
		}

		a->last_frame = last_frame;

		a->mode = mode;

		a->current_frame = 0;

		if (a->mode == EGFX_ANIMATOR_MODE__LINEAR)
		{
			a->fraction_to_move.x = ((float)a->end.x - (float)a->start.x) * fraction_to_move.x;
			a->fraction_to_move.y = ((float)a->end.y - (float)a->start.y) * fraction_to_move.y;
		}
		else
		{
			a->fraction_to_move = fraction_to_move;
		}

		egfx_invalidate_animator_object((egfx_animator_header *)a);
	}
}

void egfx_init_scalar_animator(egfx_scalar_animator *a,
	int32_t start,
	int32_t end,
	int32_t *current, //Point this to the thing you want to animate
	uint32_t last_frame,      //Maximum allowed frames before forced convergence
	float fraction_to_move, //The amount of percentage of the distance the point should move each step. Values <=0 or >= 1 are set to 0.5
	egfx_animator_mode mode
)
{
	if (a != NULL)
	{
		a->header.type = EGFX_ANIMATOR_SCALAR;
		a->header.state = EGFX_ANIMATOR_STATE_INACTIVE;
		a->start = start;
		a->end = end;

		a->__frac = (float)a->start;

		a->current = current;

		if (current != NULL)
		{
			*(a->current) = a->start;
		}

		a->last_frame = last_frame;

		a->mode = mode;

		a->current_frame = 0;

		if (a->mode == EGFX_ANIMATOR_MODE__LINEAR)
		{
			a->fraction_to_move = ((float)a->end - (float)a->start) * fraction_to_move;
		}
		else
		{
			a->fraction_to_move = fraction_to_move;
		}

		egfx_invalidate_animator_object((egfx_animator_header *)a);
	}
}


void egfx_process_point_animator(egfx_point_animator *a)
{
	if (a == NULL)
		return;

	if (a->header.state == EGFX_ANIMATOR_STATE_ACTIVE)
	{
		//Move a fraction of a distance towards the target

		if (a->mode == EGFX_ANIMATOR_MODE__FRACTIONAL_BISECT)
		{
			a->__frac.x += ((float)a->end.x - a->__frac.x) * a->fraction_to_move.x;
			a->__frac.y += ((float)a->end.y - a->__frac.y) * a->fraction_to_move.y;

			float dist_y = fabs((float)(a->end.y) - (float)(a->__frac.y));
			float dist_x = fabs((float)(a->end.x) - (float)(a->__frac.x));

			//See if we have converged or beyond the max number of steps
			if (
					(a->current_frame >= a->last_frame) ||
					(
						(dist_y < 0.5f) &&
						(dist_x < 0.5f)
					)
			  )

			{
				*(a->current) = a->end;

				egfx_animator_mark_complete(&(a->header));
			}
			else
			{
				(*(a->current)).x = (int32_t)a->__frac.x;
				(*(a->current)).y = (int32_t)a->__frac.y;
			}

		}
		else
		{
			bool x_done = false;
			bool y_done = false;

			//See if we have converged or beyond the max number of steps
			if ((a->current_frame >= a->last_frame))
			{
				*(a->current) = a->end;

				x_done = true;
				y_done = true;

			}
			else
			{

				if ((fabs((float)(a->end.x) - (float)(a->__frac.x)) >= fabs(a->fraction_to_move.x)))
				{
					a->__frac.x += a->fraction_to_move.x;

					x_done = false;
				}
				else
				{
					x_done = true;
				}

				if ((fabs((float)(a->end.y) - (float)(a->__frac.y)) >= fabs(a->fraction_to_move.y)))
				{
					a->__frac.y += a->fraction_to_move.y;
					y_done = false;
				}
				else
				{
					y_done = true;
				}
			}

			if ((x_done == true) && (y_done == true))
			{
				*(a->current) = a->end;
				egfx_animator_mark_complete(&(a->header));
			}
			else
			{
				(*(a->current)).x = (int32_t)a->__frac.x;
				(*(a->current)).y = (int32_t)a->__frac.y;
			}


		}


		a->current_frame++;

		//the linked object will get invalidated in the process routine
	}
}


void egfx_process_scalar_animator(egfx_scalar_animator *a)
{
	if (a == NULL)
		return;

	if (a->header.state == EGFX_ANIMATOR_STATE_ACTIVE)
	{
		//Move a fraction of a distance towards the target

		if (a->mode == EGFX_ANIMATOR_MODE__FRACTIONAL_BISECT)
		{
			a->__frac += ((float)a->end - a->__frac) * a->fraction_to_move;

			//See if we have converged or beyond the max number of steps
			if ((a->current_frame >= a->last_frame) ||
				((fabs((float)(a->end) - (float)(a->__frac)) <= 0.5)))
			{
				*(a->current) = a->end;
				egfx_animator_mark_complete(&(a->header));
			}
			else
			{
				*(a->current) = (int32_t)a->__frac;
			}

		}
		else
		{
			bool done = false;

			//See if we have converged or beyond the max number of steps
			if ((a->current_frame >= a->last_frame))
			{
				*(a->current) = a->end;
				done = true;
			}
			else
			{
				//Check for convergence
				if ((fabs((float)(a->end) - (float)(a->__frac)) >= fabs(a->fraction_to_move)))
				{
					a->__frac += a->fraction_to_move;
					done = false;
				}
				else
				{
					done = true;
				}
			}

			if (done == true)
			{
				*(a->current) = a->end;
				egfx_animator_mark_complete(&(a->header));
			}
			else
			{
				*(a->current) = (int32_t)a->__frac;
			}
		}

		a->current_frame++;

		//the linked object will get invalidated in the process routine
	}
}
