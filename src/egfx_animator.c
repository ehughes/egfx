
#include "egfx_animator.h"
#include "math.h"

bool eGFX_AnimatorsAreComplete(egfx_animator_header * AnimatorList)
{
	bool RetVal = true;
                
		while (AnimatorList != NULL)
		{
			if (eGFX_AnimatorIsComplete(AnimatorList) == false)
			{
				RetVal = false;
				break;
			}

			AnimatorList = ((egfx_animator_header *)(AnimatorList))->next;
		}
        
        return RetVal;
}


bool eGFX_AnimatorIsComplete(egfx_animator_header * A)
{
	bool RetVal = false;

        if(((egfx_animator_header *)(A))->state == EGFX_ANIMATOR_STATE_COMPLETE)
        {
                RetVal = true;
        }

        return RetVal;

}

void eGFX_PauseAnimator(egfx_animator_header * A)
{
        ((egfx_animator_header *)(A))->state = EGFX_OBJECT_STATE_INACTIVE;
}

void eGFX_StartAnimator(egfx_animator_header * A)
{
        (*((egfx_animator_header *)(A))).state = EGFX_OBJECT_STATE_ACTIVE;
}


void eGFX_PauseAnimators(egfx_animator_header * AnimatorList)
{
	while (AnimatorList != NULL)
	{
		eGFX_PauseAnimator(AnimatorList);
		AnimatorList = ((egfx_animator_header *)(AnimatorList))->next;
	}
}

void eGFX_StartAnimators(egfx_animator_header * AnimatorList)
{
		while (AnimatorList != NULL)
		{
			eGFX_StartAnimator(AnimatorList);
			AnimatorList = ((egfx_animator_header *)(AnimatorList))->next;
		}
}


static void eGFX_AnimatorMarkComplete(egfx_animator_header* A)
{

	A->state = EGFX_ANIMATOR_STATE_COMPLETE;

	if (A->complete != NULL)
	{
		A->complete(A->object_to_notify);
	}
}


static void eGFX_InvalidateAnimatorObject(egfx_animator_header * A)
{
	if (A != NULL)
	{
		if (A->invalidator != NULL)
		{
			A->invalidator(A->object_to_invalidate);
		}
	}
}

static void eGFX_InvalidateAllAnimatorObjects(egfx_animator_header * AnimatorList)
{
	while (AnimatorList != NULL)
	{
		eGFX_InvalidateAnimatorObject(AnimatorList);
		AnimatorList = ((egfx_animator_header *)(AnimatorList))->next;
	}
}

uint32_t eGFX_Animator_AddToList(egfx_animator_header * AnimatorList, egfx_animator_header *AnimatorToAdd)
{
	egfx_animator_header * LastObject = NULL;
	uint32_t NumObjects = 0;

	if (AnimatorList != NULL)
	{
		NumObjects = eGFX_GetAnimatorCount(AnimatorList, &LastObject);

		if ((LastObject != NULL) && (AnimatorToAdd != NULL))
		{
			((egfx_animator_header *)LastObject)->next = AnimatorToAdd;

			((egfx_animator_header *)AnimatorToAdd)->next = NULL;
		}
	}

	return NumObjects;
}

uint32_t eGFX_GetAnimatorCount(egfx_animator_header * Animator, egfx_animator_header **LastAnimator)
{
	uint32_t Count = 0;

	if (Animator != NULL)
	{
		Count = 1;
		while (
			(((egfx_object_header *)Animator)->next != NULL)
			)
		{
			Animator = ((egfx_object_header *)Animator)->next;
			Count++;
		}

		*LastAnimator = Animator;
	}

	return Count;
}


bool eGFX_Animator_Process(egfx_animator_header* A)
{
	bool Processed = true;

	if ((egfx_object_state)((egfx_animator_header*)(A))->state == (egfx_object_state)EGFX_OBJECT_STATE_ACTIVE)
	{
		switch (((egfx_animator_header*)(A))->type)
		{

		case EGFX_ANIMATOR_POINT:
			eGFX_Process_PointAnimator((egfx_point_animator*)A);
			break;

		case EGFX_ANIMATOR_SCALAR:
			eGFX_Process_ScalarAnimator((egfx_scalar_animator*)A);
			break;

		default:
			Processed = false;
			break;
		}
	}
	else
	{ 
		Processed = false;
	}

	return Processed;
}

uint32_t eGFX_Animator_ProcessList(egfx_animator_header * AnimatorList)
{
	uint32_t ProcessedAnimators = 0;
	egfx_animator_header * AnimatorStart = AnimatorList;

	//Process the list
	while (AnimatorList != NULL)
	{
		if (eGFX_Animator_Process(AnimatorList))
		{
			ProcessedAnimators++;
		}
		AnimatorList = ((egfx_object_header *)(AnimatorList))->next;
	}

	//We need to keep invalidating all animator objects until they are all done to avoid visual artifacts
	AnimatorList = AnimatorStart;
	if (eGFX_AnimatorsAreComplete(AnimatorList) == false)
	{
		eGFX_InvalidateAllAnimatorObjects(AnimatorList);
	}

	return ProcessedAnimators;
}

//Initializes the data struct
void eGFX_Init_PointAnimator(egfx_point_animator *A,
							egfx_point Start,
							egfx_point End,
							egfx_point *Current, //Point this to the thing you want to animate
							uint32_t LastFrame,      //Maximum allowed frames before forced convergence
							egfx_pointf	 FractionToMove, //The amount of percentage of the distance the point should move each step. Values <=0 or >= 1 are set to 0.5
							egfx_animator_mode Mode
						)
{
	if (A != NULL)
	{
		A->header.type = EGFX_ANIMATOR_POINT;
		A->header.state = EGFX_ANIMATOR_STATE_INACTIVE;
		A->start = Start;
		A->end = End;

		A->__frac.x = (float)A->start.x;
		A->__frac.y = (float)A->start.y;
		A->current = Current;

		if (Current != NULL)
		{
			*(A->current) = A->start;
		}

		A->last_frame = LastFrame;

		A->mode = Mode;

		A->current_frame = 0;

		if (A->mode == EGFX_ANIMATOR_MODE__LINEAR)
		{
			A->fraction_to_move.x = ((float)A->end.x - (float)A->start.x) * FractionToMove.x;
			A->fraction_to_move.y = ((float)A->end.y - (float)A->start.y) * FractionToMove.y;
		}
		else
		{
			A->fraction_to_move = FractionToMove;
		}

		eGFX_InvalidateAnimatorObject((egfx_animator_header *)A);
	}
}

void eGFX_Init_ScalarAnimator(egfx_scalar_animator *A,
	int32_t Start,
	int32_t End,
	int32_t *Current, //Point this to the thing you want to animate
	uint32_t LastFrame,      //Maximum allowed frames before forced convergence
	float	 FractionToMove, //The amount of percentage of the distance the point should move each step. Values <=0 or >= 1 are set to 0.5
	egfx_animator_mode Mode
)
{
	if (A != NULL)
	{
		A->header.type = EGFX_ANIMATOR_SCALAR;
		A->header.state = EGFX_ANIMATOR_STATE_INACTIVE;
		A->start = Start;
		A->end = End;

		A->__frac = (float)A->start;
	
		A->current = Current;

		if (Current != NULL)
		{
			*(A->current) = A->start;
		}

		A->last_frame = LastFrame;
				
		A->mode = Mode;

		A->current_frame = 0;

		if (A->mode == EGFX_ANIMATOR_MODE__LINEAR)
		{
			A->fraction_to_move = ((float)A->end - (float)A->start) * FractionToMove;
		}
		else
		{
			A->fraction_to_move = FractionToMove;
		}

		eGFX_InvalidateAnimatorObject((egfx_animator_header*)A);
	}
}


void eGFX_Process_PointAnimator(egfx_point_animator *A)
{

        if (A == NULL)
                return;

        if (A->header.state == EGFX_ANIMATOR_STATE_ACTIVE)
        {
                //Move a fraction of a distance towards the target
				
			if (A->mode == EGFX_ANIMATOR_MODE__FRACTIONAL_BISECT)
			{
				A->__frac.x += ((float)A->end.x - A->__frac.x) * A->fraction_to_move.x;
				A->__frac.y += ((float)A->end.y - A->__frac.y) * A->fraction_to_move.y;

				float dist_y = fabs((float)(A->end.y) - (float)(A->__frac.y));
				float dist_x = fabs((float)(A->end.x) - (float)(A->__frac.x));

				//See if we have converged or beyond the max number of steps
				if (
						(A->current_frame >= A->last_frame) ||
						(
							(dist_y < 0.5f) &&
							(dist_x < 0.5f)
						)
				  )
					
				{
					*(A->current) = A->end;
					
					eGFX_AnimatorMarkComplete(&(A->header));
				}
				else
				{
					(*(A->current)).x = (int32_t)A->__frac.x;
					(*(A->current)).y = (int32_t)A->__frac.y;
				}

			}
			else
			{

				bool X_Done = false;
				bool Y_Done = false;

				//See if we have converged or beyond the max number of steps
				if ((A->current_frame >= A->last_frame))
				{
					*(A->current) = A->end;

					X_Done = true;
					Y_Done = true;
				
				}
				else
				{

					if ((fabs((float)(A->end.x) - (float)(A->__frac.x)) >= fabs(A->fraction_to_move.x)))
					{
						A->__frac.x += A->fraction_to_move.x;

						X_Done = false;
					}
					else
					{
						X_Done = true;
					}

					if ((fabs((float)(A->end.y) - (float)(A->__frac.y)) >= fabs(A->fraction_to_move.y)))
					{
						A->__frac.y += A->fraction_to_move.y;
						Y_Done = false;
					}
					else
					{
						Y_Done = true;
					}
				}
				
				if ((X_Done == true) && (Y_Done == true))
				{
					*(A->current) = A->end;
					eGFX_AnimatorMarkComplete(&(A->header));
				}
				else
				{
					(*(A->current)).x = (int32_t)A->__frac.x;
					(*(A->current)).y = (int32_t)A->__frac.y;
				}


			}


            A->current_frame++;
			
    		//the linked object will get invalidated in the process routine
        }
}


void eGFX_Process_ScalarAnimator(egfx_scalar_animator *A)
{

	if (A == NULL)
		return;

	if (A->header.state == EGFX_ANIMATOR_STATE_ACTIVE)
	{
		//Move a fraction of a distance towards the target

		if (A->mode == EGFX_ANIMATOR_MODE__FRACTIONAL_BISECT)
		{
			A->__frac += ((float)A->end - A->__frac) * A->fraction_to_move;

			//See if we have converged or beyond the max number of steps
			if ((A->current_frame >= A->last_frame) ||
				((fabs((float)(A->end) - (float)(A->__frac)) <= 0.5)))
			{
				*(A->current) = A->end;
				eGFX_AnimatorMarkComplete(&(A->header));
			}
			else
			{
				*(A->current) = (int32_t)A->__frac;
			}

		}
		else
		{
			bool Done = false;

			//See if we have converged or beyond the max number of steps
			if ((A->current_frame >= A->last_frame))
			{
				*(A->current) = A->end;
				Done = true;
			}
			else
			{
				//Check for convergence
				if ((fabs((float)(A->end) - (float)(A->__frac)) >= fabs(A->fraction_to_move)))
				{
					A->__frac += A->fraction_to_move;
					Done = false;
				}
				else
				{
					Done = true;
				}
			}

			if (Done == true)
			{
				*(A->current) = A->end;
				eGFX_AnimatorMarkComplete(&(A->header));
			}
			else
			{
				*(A->current) = (int32_t)A->__frac;
			}
		}

		A->current_frame++;

		//the linked object will get invalidated in the process routine
	}
}
