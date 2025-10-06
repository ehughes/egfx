#ifdef __cplusplus
extern "C" {
#endif

#ifndef eGFX_PLANE_H
#define eGFX_PLANE_H
     
     

     
     
uint8_t EGFX_WEAK egfx_get_bit_from_array(uint8_t *DataArray,
                                          uint16_t Bit);

uint8_t EGFX_WEAK egfx_image_plane_get_byte(egfx_img   *Image,
                                            uint16_t X,
                                            uint16_t Y);

void EGFX_WEAK egfx_image_plane_init(egfx_img* Image,
                                      uint8_t* Store,
                                      int16_t SizeX,
                                      int16_t SizeY,
                                      uint32_t Type);

void EGFX_WEAK egfx_fill(egfx_img *Image, egfx_pixel_state color);


#endif

#ifdef __cplusplus
 }
#endif
