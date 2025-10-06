
#include "egfx_types.h"

#ifdef __cplusplus
extern "C" {
#endif

#ifndef eGFX_PRIMITIVE_OPS_H
#define eGFX_PRIMITIVE_OPS_H

void  eGFX_DrawH_Line(egfx_img *Image,
                               int32_t X_Start,
                               int32_t X_Stop,
                               int32_t Y,
                               egfx_pixel_state PS);

void EGFX_WEAK eGFX_DrawV_Line(egfx_img *Image,
                              int32_t Y_Start,
                              int32_t YStop,
                              int32_t X,
                              egfx_pixel_state PS);    

void EGFX_WEAK eGFX_DrawSolidRectangle(egfx_img *Image,
							egfx_rect * R,
							egfx_pixel_state PS);


void EGFX_WEAK eGFX_DrawFilledBox(egfx_img *Image,
                                  egfx_box *Box,
                                  egfx_pixel_state PS);                              
                              
void EGFX_WEAK eGFX_DrawBox(egfx_img *Image,
                            egfx_box *Box,
                            egfx_pixel_state PS);

void EGFX_WEAK eGFX_DrawLine(egfx_img *Image,
                             int32_t X1,
                             int32_t Y1,
                             int32_t X2,
                             int32_t Y2,
                             egfx_pixel_state PS);      

void EGFX_WEAK eGFX_DrawCircle(egfx_img *Image,
                               int32_t x0,
                               int32_t y0,
                               int32_t radius,
                               egfx_pixel_state PS);      

void EGFX_WEAK eGFX_DrawCircleFromBackground(egfx_img *Image,
	egfx_img *Background,
	int32_t x0,
	int32_t y0,
	int32_t radius);

void EGFX_WEAK eGFX_DrawFilledCircle(egfx_img *Image,
                                    int16_t x0,
                                    int16_t y0,
                                    int16_t radius,
                                    egfx_pixel_state PS);                               
         
#endif

#ifdef __cplusplus
 }
#endif
