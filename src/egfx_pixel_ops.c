#include "egfx_config.h"
#include "egfx_types.h"
#include "egfx_pixel_ops.h"
#include "egfx_plane.h"
#include "string.h"
#include "stdint.h"

// Helper function to apply image transformation flags
// Transforms logical coordinates (x, y) to physical memory coordinates
// Also returns the physical dimensions (mem_size_x, mem_size_y)
//
// Note: img->size_x and img->size_y ALWAYS represent the physical memory layout.
// When TRANSPOSE is set, user coordinates are interpreted as being in the swapped (logical) space.
static inline void egfx_apply_transform(const egfx_img *img,
                                         uint16_t *x, uint16_t *y,
                                         uint16_t *mem_size_x, uint16_t *mem_size_y)
{
    uint16_t tx = *x;
    uint16_t ty = *y;

    // Determine logical dimensions (what user coordinates are relative to)
    uint16_t logical_size_x, logical_size_y;
    if (img->flags & EGFX_IMG_FLAGS_TRANSPOSE) {
        // When transposed, logical dimensions are swapped from physical
        logical_size_x = img->size_y;
        logical_size_y = img->size_x;
    } else {
        // No transpose, logical = physical
        logical_size_x = img->size_x;
        logical_size_y = img->size_y;
    }

    // Apply flips using logical dimensions
    if (img->flags & EGFX_IMG_FLAGS_FLIP_X) {
        tx = logical_size_x - 1 - tx;
    }

    if (img->flags & EGFX_IMG_FLAGS_FLIP_Y) {
        ty = logical_size_y - 1 - ty;
    }

    // Apply transpose (swap coordinates to map back to physical)
    if (img->flags & EGFX_IMG_FLAGS_TRANSPOSE) {
        uint16_t temp = tx;
        tx = ty;
        ty = temp;
    }

    // Physical dimensions for memory access are always img->size_x/size_y
    *mem_size_x = img->size_x;
    *mem_size_y = img->size_y;

    *x = tx;
    *y = ty;
}

void EGFX_WEAK egfx_bit_stream_put(uint8_t *BitStream,
                                 uint32_t Position,
                                 uint32_t PixelState)
{
        uint32_t BaseAddress = Position >> 3;
        uint32_t Offset = Position & 0x7;

        if(PixelState)
        {
            BitStream[BaseAddress] |= (1<<Offset);
        }
        else
        {
            BitStream[BaseAddress] &= ~(1<<Offset);
        }
}

uint32_t EGFX_WEAK egfx_bit_stream_get(uint8_t * BitStream, uint32_t Position)
{
    uint32_t BaseAddress = Position >> 3;
    uint32_t Offset = Position & 0x7;

    return (BitStream[BaseAddress] & (1<<Offset));
}

egfx_put_pixel_t * egfx_get_put_pixel_func(const egfx_img *Image)
{
        egfx_put_pixel_t *put_pixel_func;

        switch(EGFX_GET_BPP_FROM_IMG(Image))
        {
            case  1:
                    put_pixel_func = (egfx_put_pixel_t *)egfx_put_pixel_1bpp;
                break;

            case  3:
                    put_pixel_func = (egfx_put_pixel_t *)egfx_put_pixel_3bpp;
               break;

            case  4:
                    put_pixel_func = (egfx_put_pixel_t *)egfx_put_pixel_4bpp;
                break;

            case  8:
                    put_pixel_func = (egfx_put_pixel_t *)egfx_put_pixel_8bpp;
                break;

            case  16:
                    put_pixel_func = (egfx_put_pixel_t *)egfx_put_pixel_16bpp;
                break;

            case  24:
                    put_pixel_func = (egfx_put_pixel_t *)egfx_put_pixel_24bpp;
                break;

            case  32:
                   put_pixel_func = (egfx_put_pixel_t *)egfx_put_pixel_32bpp;
               break;

            default:
                   put_pixel_func = (egfx_put_pixel_t *)egfx_put_pixel_stub;
                break;
        }

        return put_pixel_func;
}


void EGFX_WEAK egfx_put_pixel_stub(const egfx_img *Image,
                                  uint16_t x,
                                  uint16_t y,
                                  egfx_pixel_state ps)
 {

 }

void EGFX_WEAK egfx_put_pixel_1bpp(const egfx_img *Image,
                                  uint16_t x,
                                  uint16_t y,
                                  egfx_pixel_state ps)
 {
    uint16_t mem_size_x, mem_size_y;

    // Apply transformation (transpose, flip_x, flip_y)
    egfx_apply_transform(Image, &x, &y, &mem_size_x, &mem_size_y);

    // Use raw pixel operation
    egfx_pp_1bpp(Image->data, mem_size_x, x, y, ps);
 }                                 



void EGFX_WEAK egfx_put_pixel_3bpp(const egfx_img *Image,
                                  uint16_t x,
                                  uint16_t y,
                                  egfx_pixel_state ps)

 {
    //This image plane type is an odd ball case for the 3-color sharp memory LCD.
    uint16_t mem_size_x, mem_size_y;

    // Apply transformation (transpose, flip_x, flip_y)
    egfx_apply_transform(Image, &x, &y, &mem_size_x, &mem_size_y);

    // Use raw pixel operation
    egfx_pp_3bpp(Image->data, mem_size_x, x, y, ps);
 }                                


 void EGFX_WEAK egfx_put_pixel_4bpp(const egfx_img *Image,
                                  uint16_t x,
                                  uint16_t y,
                                  egfx_pixel_state ps)
 {
    uint16_t mem_size_x, mem_size_y;

    // Apply transformation (transpose, flip_x, flip_y)
    egfx_apply_transform(Image, &x, &y, &mem_size_x, &mem_size_y);

    // Use raw pixel operation
    egfx_pp_4bpp(Image->data, mem_size_x, x, y, ps);
 }                                 



void EGFX_WEAK egfx_put_pixel_8bpp(const egfx_img *Image,
                                  uint16_t x,
                                  uint16_t y,
                                  egfx_pixel_state ps)
 {
    uint16_t mem_size_x, mem_size_y;

    // Apply transformation (transpose, flip_x, flip_y)
    egfx_apply_transform(Image, &x, &y, &mem_size_x, &mem_size_y);

    // Use raw pixel operation
    egfx_pp_8bpp(Image->data, mem_size_x, x, y, ps);
 }                                 



void EGFX_WEAK egfx_put_pixel_16bpp(const egfx_img *Image,
                                  uint16_t x,
                                  uint16_t y,
                                  egfx_pixel_state ps)
 {
    uint16_t mem_size_x, mem_size_y;

    // Apply transformation (transpose, flip_x, flip_y)
    egfx_apply_transform(Image, &x, &y, &mem_size_x, &mem_size_y);

    // Use raw pixel operation
    egfx_pp_16bpp(Image->data, mem_size_x, x, y, ps);
 }                                 



void EGFX_WEAK egfx_put_pixel_24bpp(const egfx_img *Image,
                                  uint16_t x,
                                  uint16_t y,
                                  egfx_pixel_state ps)
 {
    uint16_t mem_size_x, mem_size_y;

    // Apply transformation (transpose, flip_x, flip_y)
    egfx_apply_transform(Image, &x, &y, &mem_size_x, &mem_size_y);

    // Use raw pixel operation
    egfx_pp_24bpp(Image->data, mem_size_x, x, y, ps);
 }                                 

 

void EGFX_WEAK egfx_put_pixel_32bpp(const egfx_img *Image,
                                  uint16_t x,
                                  uint16_t y,
                                  egfx_pixel_state ps)
 {
    uint16_t mem_size_x, mem_size_y;

    // Apply transformation (transpose, flip_x, flip_y)
    egfx_apply_transform(Image, &x, &y, &mem_size_x, &mem_size_y);

    // Use raw pixel operation
    egfx_pp_32bpp(Image->data, mem_size_x, x, y, ps);
 }                                 

                              
void EGFX_WEAK egfx_put_pixel(const egfx_img *Image,
                             uint16_t x,
                             uint16_t y,
                             egfx_pixel_state ps)
{
    // Determine logical bounds (what the user draws in)
    uint16_t logical_max_x, logical_max_y;
    if (Image->flags & EGFX_IMG_FLAGS_TRANSPOSE) {
        // When transposed, logical dimensions are swapped from physical
        logical_max_x = Image->size_y;
        logical_max_y = Image->size_x;
    } else {
        logical_max_x = Image->size_x;
        logical_max_y = Image->size_y;
    }

    if((x < logical_max_x) && (y < logical_max_y) && (x >= 0) && (y >= 0))
    {
        egfx_put_pixel_t *put_pixel_func = egfx_get_put_pixel_func(Image);
        put_pixel_func(Image, x, y, ps);
    }
}


egfx_pixel_state EGFX_WEAK egfx_get_pixel(const egfx_img *Image,
                                        uint16_t x,
                                        uint16_t y)
{
    egfx_pixel_state ps = 0;

    // Determine logical bounds (what the user reads from)
    uint16_t logical_max_x, logical_max_y;
    if (Image->flags & EGFX_IMG_FLAGS_TRANSPOSE) {
        // When transposed, logical dimensions are swapped from physical
        logical_max_x = Image->size_y;
        logical_max_y = Image->size_x;
    } else {
        logical_max_x = Image->size_x;
        logical_max_y = Image->size_y;
    }

    if((x < logical_max_x) && (y < logical_max_y) && (x >= 0) && (y >= 0))
    {
        egfx_get_pixel_t *get_pixel_func = egfx_get_get_pixel_func(Image);
        ps = get_pixel_func(Image, x, y);
    }

    return ps;
}


egfx_get_pixel_t *egfx_get_get_pixel_func(const egfx_img *Image)
{
        egfx_get_pixel_t *get_pixel_func;

        switch(EGFX_GET_BPP_FROM_IMG(Image))
        {
            case  1:
                    get_pixel_func = (egfx_get_pixel_t * )egfx_get_pixel_1bpp;
                break;

            case  3:
                    get_pixel_func = (egfx_get_pixel_t * )egfx_get_pixel_3bpp;
               break;

            case  4:
                    get_pixel_func = (egfx_get_pixel_t * )egfx_get_pixel_4bpp;
                break;

            case  8:
                    get_pixel_func = (egfx_get_pixel_t * )egfx_get_pixel_8bpp;
                break;

            case  16:
                    get_pixel_func = (egfx_get_pixel_t * )egfx_get_pixel_16bpp;
                break;

            case  24:
                    get_pixel_func = (egfx_get_pixel_t * )egfx_get_pixel_24bpp;
                break;

            case  32:
                   get_pixel_func = (egfx_get_pixel_t * )egfx_get_pixel_32bpp;
               break;

            default:
                   get_pixel_func = (egfx_get_pixel_t * )egfx_get_pixel_stub;
                break;
        }

        return get_pixel_func;
}
        

egfx_pixel_state EGFX_WEAK egfx_get_pixel_stub(const egfx_img* Image,
    uint16_t x,
    uint16_t y
)
{
    return 0;
}

egfx_pixel_state EGFX_WEAK egfx_get_pixel_1bpp(const egfx_img *Image,
                                  uint16_t x,
                                  uint16_t y
                                  )
 {
    uint16_t mem_size_x, mem_size_y;

    // Apply transformation (transpose, flip_x, flip_y)
    egfx_apply_transform(Image, &x, &y, &mem_size_x, &mem_size_y);

    // Use raw pixel operation
    return egfx_gp_1bpp(Image->data, mem_size_x, x, y);
 }


egfx_pixel_state EGFX_WEAK egfx_get_pixel_3bpp(const egfx_img *Image,
                                  uint16_t x,
                                  uint16_t y
)
{
    uint16_t mem_size_x, mem_size_y;

    // Apply transformation (transpose, flip_x, flip_y)
    egfx_apply_transform(Image, &x, &y, &mem_size_x, &mem_size_y);

    // Use raw pixel operation
    return egfx_gp_3bpp(Image->data, mem_size_x, x, y);
}                                 



egfx_pixel_state EGFX_WEAK egfx_get_pixel_4bpp(const egfx_img *Image,
                                  uint16_t x,
                                  uint16_t y
                                  )
 {
    uint16_t mem_size_x, mem_size_y;

    // Apply transformation (transpose, flip_x, flip_y)
    egfx_apply_transform(Image, &x, &y, &mem_size_x, &mem_size_y);

    // Use raw pixel operation
    return egfx_gp_4bpp(Image->data, mem_size_x, x, y);
 }                                 



egfx_pixel_state EGFX_WEAK egfx_get_pixel_8bpp(const egfx_img *Image,
    uint16_t x,
                                  uint16_t y
                                  )
 {
    uint16_t mem_size_x, mem_size_y;

    // Apply transformation (transpose, flip_x, flip_y)
    egfx_apply_transform(Image, &x, &y, &mem_size_x, &mem_size_y);

    // Use raw pixel operation
    return egfx_gp_8bpp(Image->data, mem_size_x, x, y);
 }



egfx_pixel_state EGFX_WEAK egfx_get_pixel_16bpp(const egfx_img *Image,
                                  uint16_t x,
                                  uint16_t y
                                  )
 {
    uint16_t mem_size_x, mem_size_y;

    // Apply transformation (transpose, flip_x, flip_y)
    egfx_apply_transform(Image, &x, &y, &mem_size_x, &mem_size_y);

    // Use raw pixel operation
    return egfx_gp_16bpp(Image->data, mem_size_x, x, y);
 }



egfx_pixel_state EGFX_WEAK egfx_get_pixel_24bpp(const egfx_img *Image,
                                  uint16_t x,
                                  uint16_t y
                                  )
{
    uint16_t mem_size_x, mem_size_y;

    // Apply transformation (transpose, flip_x, flip_y)
    egfx_apply_transform(Image, &x, &y, &mem_size_x, &mem_size_y);

    // Use raw pixel operation
    return egfx_gp_24bpp(Image->data, mem_size_x, x, y);
}

egfx_pixel_state EGFX_WEAK egfx_get_pixel_32bpp(const egfx_img *Image,
                                  uint16_t x,
                                  uint16_t y
                                  )
{
    uint16_t mem_size_x, mem_size_y;

    // Apply transformation (transpose, flip_x, flip_y)
    egfx_apply_transform(Image, &x, &y, &mem_size_x, &mem_size_y);

    // Use raw pixel operation
    return egfx_gp_32bpp(Image->data, mem_size_x, x, y);
}
