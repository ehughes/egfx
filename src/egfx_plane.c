#include "egfx_config.h"
#include "egfx_types.h"
#include "egfx_pixel_ops.h"
#include "egfx_blit.h"
#include "egfx_plane.h"

// Fast memset using 32-bit writes
static inline void egfx_fast_memset32(uint32_t *dest, uint32_t value, uint32_t count)
{
    while(count--)
    {
        *dest++ = value;
    }
}


uint8_t EGFX_WEAK egfx_get_bit_from_array(uint8_t *DataArray,uint16_t Bit)
{
    uint8_t Mask;
    Mask = 1 << (7-(Bit&0x7));

    if(DataArray[Bit>>3] & Mask)
        return 1;
    else
        return 0;
}

//This function grabs an 8-bit chunk from each row of an image plane.   Assumes we are byte aligned and is really only used for 1BPP and 4BPP
//Image types to assist a driver
//X is the starting pixel.   For 1BPP It will be byte aligned. I.e.  requesting X = 12 will get you the byte for X starting at 8.
//for 4BPP, requesting X = 13 will get you the byte for X starting for 12

uint8_t EGFX_WEAK egfx_image_plane_get_byte(egfx_img   *Image,
                                            uint16_t X,
                                            uint16_t Y)
{
    uint32_t Offset;
    uint32_t MemWidthInBytes;
    uint8_t RetVal = 0;

    switch(EGFX_IMG_GET_TYPE(Image))
    {
        case EGFX_IMG_4BPP_XBGR:
        case EGFX_IMG_4BPP:

                  if(X > Image->size_x)   //Make sure we aren't asking for a chunk greater than the width of one row
                    {
                                X = (Image->size_x - 1);
                    }
                  if(Y > Image->size_y)
                    {
                                  X = (Image->size_y- - 1);
                    }

                    X &= ~((uint16_t)0x1); // Make sure we are on a byte boundary

                    //Properly Implement Rounding for odd bit plane sizes
                      MemWidthInBytes  = (Image->size_x)>>1;

                      if(Image->size_x & 0x1)
                                MemWidthInBytes++;

                      Offset = (Y * MemWidthInBytes) + (X>>1);
                      RetVal = Image->data[Offset];

                break;

        case EGFX_IMG_1BPP:

                  if(X > Image->size_x)   //Make sure we aren't asking for a chunk greater than the width of one row
                    {
                        X = (Image->size_x - 1);
                    }

                  if(Y > Image->size_y)
                  {
                          X = (Image->size_y- - 1);
                  }

                    X &= ~((uint16_t)0x7); // Make sure we are on a byte boundary
                    //Properly Implement Rounding for odd bit plane sizes
                    MemWidthInBytes  = (Image->size_x)>>3;

                    if(Image->size_x & 0x7)
                        MemWidthInBytes++;

                    Offset = (Y * MemWidthInBytes) + (X>>3);
                    RetVal = Image->data[Offset];

                    break;

        default:

                // Other image planes not supported
                

                break;
    }
                
                return RetVal;
}

void EGFX_WEAK egfx_image_plane_init(egfx_img   *Image,
                                      uint8_t * Store,
                                      int16_t SizeX,
                                      int16_t SizeY,
                                      uint32_t Type)
{
    Image->data = Store;
    Image->size_x = SizeX;
    Image->size_y = SizeY;
    Image->bits_per_pixel = (Type >> 8) & 0xFF;
    Image->color_type = Type & 0xFF;
    Image->flags = 0;
    Image->rsv = 0;
}

void EGFX_WEAK egfx_fill(egfx_img *Image, egfx_pixel_state color)
{
    uint32_t pixel_count = Image->size_x * Image->size_y;
    uint32_t byte_count;
    uint32_t word_count;
    uint32_t remaining_bytes;
    uint32_t fill_word;
    uint32_t i;
    uint8_t *byte_ptr;

    switch(Image->bits_per_pixel)
    {
        case 1:
        {
            // For 1BPP, fill with 0 or 0xFF pattern
            byte_count = EGFX_CALC_1BPP_IMG_STORAGE_SPACE_SIZE(Image->size_x, Image->size_y);
            fill_word = color ? 0xFFFFFFFF : 0x00000000;

            word_count = byte_count >> 2;
            remaining_bytes = byte_count & 0x3;

            egfx_fast_memset32((uint32_t *)Image->data, fill_word, word_count);

            // Handle remaining bytes
            byte_ptr = Image->data + (word_count << 2);
            for(i = 0; i < remaining_bytes; i++)
            {
                byte_ptr[i] = (uint8_t)fill_word;
            }
            break;
        }

        case 3:
        {
            // 3BPP - special case, not commonly used
            // Fall through to default
            break;
        }

        case 4:
        {
            // Pack 2 pixels per byte, then replicate to 32 bits
            uint8_t fill_byte = ((color & 0x0F) << 4) | (color & 0x0F);
            fill_word = (fill_byte << 24) | (fill_byte << 16) | (fill_byte << 8) | fill_byte;

            byte_count = EGFX_CALC_4BPP_IMG_STORAGE_SPACE_SIZE(Image->size_x, Image->size_y);
            word_count = byte_count >> 2;
            remaining_bytes = byte_count & 0x3;

            egfx_fast_memset32((uint32_t *)Image->data, fill_word, word_count);

            // Handle remaining bytes
            byte_ptr = Image->data + (word_count << 2);
            for(i = 0; i < remaining_bytes; i++)
            {
                byte_ptr[i] = fill_byte;
            }
            break;
        }

        case 8:
        {
            // Replicate byte to 32 bits
            uint8_t fill_byte = color & 0xFF;
            fill_word = (fill_byte << 24) | (fill_byte << 16) | (fill_byte << 8) | fill_byte;

            byte_count = pixel_count;
            word_count = byte_count >> 2;
            remaining_bytes = byte_count & 0x3;

            egfx_fast_memset32((uint32_t *)Image->data, fill_word, word_count);

            // Handle remaining bytes
            byte_ptr = Image->data + (word_count << 2);
            for(i = 0; i < remaining_bytes; i++)
            {
                byte_ptr[i] = fill_byte;
            }
            break;
        }

        case 16:
        {
            // Replicate 16-bit value to 32 bits
            uint16_t fill_pixel = color & 0xFFFF;
            fill_word = (fill_pixel << 16) | fill_pixel;

            byte_count = pixel_count << 1;
            word_count = byte_count >> 2;
            remaining_bytes = byte_count & 0x3;

            egfx_fast_memset32((uint32_t *)Image->data, fill_word, word_count);

            // Handle remaining bytes (should be 0 or 2 bytes for 16bpp)
            if(remaining_bytes)
            {
                uint16_t *pixel_ptr = (uint16_t *)(Image->data + (word_count << 2));
                *pixel_ptr = fill_pixel;
            }
            break;
        }

        case 24:
        {
            // 24BPP doesn't align well with 32-bit writes, use byte writes
            uint8_t r = (color >> 16) & 0xFF;
            uint8_t g = (color >> 8) & 0xFF;
            uint8_t b = color & 0xFF;

            for(i = 0; i < pixel_count; i++)
            {
                Image->data[i * 3 + 0] = r;
                Image->data[i * 3 + 1] = g;
                Image->data[i * 3 + 2] = b;
            }
            break;
        }

        case 32:
        {
            // Direct 32-bit writes
            fill_word = color;
            egfx_fast_memset32((uint32_t *)Image->data, fill_word, pixel_count);
            break;
        }

        default:
            break;
    }
}
