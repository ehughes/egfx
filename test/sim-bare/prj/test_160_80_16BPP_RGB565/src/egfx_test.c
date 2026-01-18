/**
 * @file egfx_test.c
 * @brief Simulator test for 160x80 RGB565 display (ST7735R charger tester)
 *
 * This test simulates the charger tester display for UI development
 * and regression testing.
 */

#include <stdlib.h>
#include <stdio.h>
#include <time.h>
#include "egfx.h"
#include "egfx_color.h"
#include "egfx_primitive_ops.h"
#include "sprites.h"
#include "eGFX_DisplayDriver_ImGui.h"

// RGB565 color helpers
#define RGB565(r, g, b) ((((r) & 0xF8) << 8) | (((g) & 0xFC) << 3) | (((b) & 0xF8) >> 3))
#define RED_565     RGB565(255, 0, 0)
#define GREEN_565   RGB565(0, 255, 0)
#define BLUE_565    RGB565(0, 0, 255)
#define WHITE_565   RGB565(255, 255, 255)
#define YELLOW_565  RGB565(255, 255, 0)
#define CYAN_565    RGB565(0, 255, 255)
#define MAGENTA_565 RGB565(255, 0, 255)

int main(int argc, char *argv[])
{
    egfx_init_driver(NULL);

    int frame = 0;

    while (!ProcessSimEvents())
    {
        // Draw mr_cricket sprite - fills entire 160x80 screen
        egfx_blit(egfx_back_buffer[0],
                  &mr_cricket,
                  (egfx_point){.x = 0, .y = 0});

        // Draw some demonstration primitives

        // Horizontal line at top
        egfx_draw_h_line(egfx_back_buffer[0], 0, 159, 0, RED_565);

        // Vertical line on left
        egfx_draw_v_line(egfx_back_buffer[0], 0, 79, 0, GREEN_565);

        // Diagonal lines forming an X in bottom-right corner
        egfx_draw_line(egfx_back_buffer[0], 120, 50, 155, 75, YELLOW_565);
        egfx_draw_line(egfx_back_buffer[0], 155, 50, 120, 75, CYAN_565);

        // Circle in bottom-left area
        egfx_draw_circle(egfx_back_buffer[0], 25, 55, 15, MAGENTA_565);

        // Filled circle nearby
        egfx_draw_filled_circle(egfx_back_buffer[0], 60, 55, 10, BLUE_565);

        // Box outline
        egfx_box box = {.p1 = {.x = 100, .y = 12}, .p2 = {.x = 155, .y = 35}};
        egfx_draw_box(egfx_back_buffer[0], &box, WHITE_565);

        // Overlay text on top of the sprite
        egfx_text(egfx_back_buffer[0], "Charger Tester", (egfx_point){5, 2},
            &(egfx_text_config){
                .font = &font_5_7,
                .colored = true,
                .color = {255, 255, 255},
                .antialiased = false
            });

        // Status text at bottom
        egfx_text(egfx_back_buffer[0], "160x80 RGB565", (egfx_point){70, 70},
            &(egfx_text_config){
                .font = &font_3_5,
                .colored = true,
                .color = {255, 255, 0},
                .antialiased = false
            });

        egfx_dump(egfx_back_buffer[0]);
        frame++;
    }

    egfx_deinit_driver();

    return 0;
}
