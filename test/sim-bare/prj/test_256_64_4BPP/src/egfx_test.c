#include <stdlib.h>
#include <stdio.h>
#include <time.h>
#include "egfx.h"
#include "sprites.h"
#include "eGFX_DisplayDriver_ImGui.h"

egfx_animator_header* Animators;

egfx_point_animator A[6];

egfx_scalar_animator LogoAnimator;

egfx_point Line1;
egfx_point Line2;
egfx_point Line3;
egfx_point Line4;

egfx_point SeparatorStart;
egfx_point SeparatorEnd;

#define TITLE_Y  4
#define TITLE_X  96

#define INFO_Y          34
#define INFO_Y_SPACE    9

#define SEPARATOR_X     TITLE_X - 8

int32_t logo_scale_int;
int AnimState;


void init()
{
      eGFX_Init_ScalarAnimator(&LogoAnimator,
        0,
        255,
        &logo_scale_int, //Point this to the thing you want to animate
        120,      //Maximum allowed frames before forced convergence
        .15f, //The amount of percentage of the distance the point should move each step. SHould be small (<0.5)
        EGFX_ANIMATOR_MODE__FRACTIONAL_BISECT
    );

    eGFX_Init_PointAnimator(&A[0],
        (egfx_point){.x = 300, .y = 64}, //Start
        (egfx_point){.x = TITLE_X, .y = TITLE_Y}, //End
        &Line1, //Point this to the thing you want to animate
        1000,      //Maximum allowed frames before forced convergence
        (egfx_pointf){.15f,.15f}, //The amount of percentage of the distance the point should move each step. SHould be small (<0.5)
          EGFX_ANIMATOR_MODE__FRACTIONAL_BISECT
    );

    eGFX_Init_PointAnimator(&A[1],
        (egfx_point) { .x = 32, .y = 88 }, //Start
        (egfx_point) { .x = TITLE_X, .y = INFO_Y + (0*INFO_Y_SPACE) }, //End
        & Line2, //Point this to the thing you want to animate
        1000,      //Maximum allowed frames before forced convergence
        (egfx_pointf) {.15f, .15f }, //The amount of percentage of the distance the point should move each step. SHould be small (<0.5)
        EGFX_ANIMATOR_MODE__FRACTIONAL_BISECT
    );

    eGFX_Init_PointAnimator(&A[2],
        (egfx_point) { .x = 32, .y = 88 }, //Start
        (egfx_point) { .x = 96, .y = INFO_Y + (1*INFO_Y_SPACE)}, //End
        &Line3, //Point this to the thing you want to animate
        1000,      //Maximum allowed frames before forced convergence
        (egfx_pointf) {.15f, .15f }, //The amount of percentage of the distance the point should move each step. SHould be small (<0.5)
        EGFX_ANIMATOR_MODE__FRACTIONAL_BISECT
    );

    eGFX_Init_PointAnimator(&A[3],
        (egfx_point) { .x = 32, .y = 88 }, //Start
        (egfx_point) { .x = TITLE_X, .y = INFO_Y + (2*INFO_Y_SPACE) }, //End
        &Line4, //Point this to the thing you want to animate
        1000,      //Maximum allowed frames before forced convergence
        (egfx_pointf) {.15f, .15f }, //The amount of percentage of the distance the point should move each step. SHould be small (<0.5)
        EGFX_ANIMATOR_MODE__FRACTIONAL_BISECT
    );

    eGFX_Init_PointAnimator(&A[4],
        (egfx_point){.x = 300, .y = 64}, //Start
        (egfx_point){.x = SEPARATOR_X, .y = 4}, //End
        &SeparatorStart, //Point this to the thing you want to animate
        1000,      //Maximum allowed frames before forced convergence
        (egfx_pointf){.15f,.15f}, //The amount of percentage of the distance the point should move each step. SHould be small (<0.5)
          EGFX_ANIMATOR_MODE__FRACTIONAL_BISECT
    );

    eGFX_Init_PointAnimator(&A[5],
        (egfx_point) { .x = 200, .y = 0 }, //Start
        (egfx_point) { .x = SEPARATOR_X, .y = 60  }, //End
        & SeparatorEnd, //Point this to the thing you want to animate
        1000,      //Maximum allowed frames before forced convergence
        (egfx_pointf) {.15f, .15f }, //The amount of percentage of the distance the point should move each step. SHould be small (<0.5)
        EGFX_ANIMATOR_MODE__FRACTIONAL_BISECT
    );

   Animators = (egfx_animator_header*)&A[0];

    eGFX_Animator_AddToList(Animators,(egfx_animator_header*)&A[1]);
    eGFX_Animator_AddToList(Animators,(egfx_animator_header*)&A[2]);
    eGFX_Animator_AddToList(Animators,(egfx_animator_header*)&A[3]);
    eGFX_Animator_AddToList(Animators,(egfx_animator_header*)&A[4]);
    eGFX_Animator_AddToList(Animators,(egfx_animator_header*)&A[5]);
    eGFX_Animator_AddToList(Animators,(egfx_animator_header*)&LogoAnimator);


}

int main(int argc, char *argv[])
{
  
    eGFX_InitDriver(NULL);

    init();

    eGFX_StartAnimators(Animators);

    // Replace ProcessSDL_Events with ProcessEvents
    while (!ProcessSimEvents())
    {




        egfx_fill(eGFX_BackBuffer[0], 0);

        eGFX_Animator_ProcessList(Animators);

        egfx_text(eGFX_BackBuffer[0], "Chronos", Line1, &(egfx_text_config){.font = &font_10_14, .colored = false});

        egfx_text(eGFX_BackBuffer[0], "Serial: 10092", Line2, &(egfx_text_config){.font = &font_5_7, .colored = false});

        egfx_text(eGFX_BackBuffer[0], "Firmware Version : v2.1", Line3, &(egfx_text_config){.font = &font_5_7, .colored = false});

        egfx_text(eGFX_BackBuffer[0], "Bootloader : v1.1", Line4, &(egfx_text_config){.font = &font_5_7, .colored = false});

        eGFX_DrawLine(eGFX_BackBuffer[0], SeparatorStart.x, SeparatorStart.y, SeparatorEnd.x, SeparatorEnd.y, 0xFF);

        egfx_blit_faded(eGFX_BackBuffer[0], (egfx_img *)&wn_w_text_83_56, (egfx_point){8, 4}, logo_scale_int);

        AnimState++;
        if (AnimState > 80)
        {

            if (eGFX_AnimatorsAreComplete(Animators))
            {

                for (int i = 0; i < 6; i++)
                {
                    A[i].end.y = 350;
                    A[i].start = *(A[i].current);
                    A[i].current_frame = 0;

                    A[i].fraction_to_move.x = .025;
                    A[i].fraction_to_move.y = .025;
                }

                A[4].end.x = 400;
                A[4].end.y = 64;

                A[4].fraction_to_move.x = .05;
                A[4].fraction_to_move.y = .05;

                A[5].end.x = 400;
                A[5].end.y = 0;

                A[5].fraction_to_move.x = .15;
                A[5].fraction_to_move.y = .15;

                LogoAnimator.end = 0;
                LogoAnimator.current_frame = 0;
                LogoAnimator.start = *LogoAnimator.current;
                eGFX_StartAnimators(Animators);
            }
        }
        if (AnimState > 160)
        {
            AnimState = 0;
            init();
            eGFX_StartAnimators(Animators);

        }

        eGFX_Dump(eGFX_BackBuffer[0]);
    }

    eGFX_DeInitDriver();
    
    return 0;
}