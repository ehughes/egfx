
#include "egfx_types.h"

#ifdef __cplusplus
extern "C" {
#endif

#ifndef EGFX_PRIMITIVE_OPS_H
#define EGFX_PRIMITIVE_OPS_H

void EGFX_WEAK egfx_draw_h_line(egfx_img *image,
                                int32_t x_start,
                                int32_t x_stop,
                                int32_t y,
                                egfx_pixel_state ps);

void EGFX_WEAK egfx_draw_v_line(egfx_img *image,
                                int32_t y_start,
                                int32_t y_stop,
                                int32_t x,
                                egfx_pixel_state ps);

void EGFX_WEAK egfx_draw_solid_rectangle(egfx_img *image,
                                         egfx_rect *r,
                                         egfx_pixel_state ps);

void EGFX_WEAK egfx_draw_filled_box(egfx_img *image,
                                    egfx_box *box,
                                    egfx_pixel_state ps);

void EGFX_WEAK egfx_draw_box(egfx_img *image,
                             egfx_box *box,
                             egfx_pixel_state ps);

void EGFX_WEAK egfx_draw_line(egfx_img *image,
                              int32_t x1,
                              int32_t y1,
                              int32_t x2,
                              int32_t y2,
                              egfx_pixel_state ps);

void EGFX_WEAK egfx_draw_circle(egfx_img *image,
                                int32_t x0,
                                int32_t y0,
                                int32_t radius,
                                egfx_pixel_state ps);

void EGFX_WEAK egfx_draw_circle_from_background(egfx_img *image,
                                                egfx_img *background,
                                                int32_t x0,
                                                int32_t y0,
                                                int32_t radius);

void EGFX_WEAK egfx_draw_filled_circle(egfx_img *image,
                                       int16_t x0,
                                       int16_t y0,
                                       int16_t radius,
                                       egfx_pixel_state ps);

#endif

#ifdef __cplusplus
 }
#endif
