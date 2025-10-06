#include "egfx.h"
#include <string.h>

#ifdef __cplusplus
extern "C" {
#endif

#ifndef eGFX_PIXEL_OPS_H
#define eGFX_PIXEL_OPS_H

// Forward declarations for functions used by inline operations
void EGFX_WEAK egfx_bit_stream_put(uint8_t * BitStream, uint32_t Position, uint32_t PixelState);
uint32_t EGFX_WEAK egfx_bit_stream_get(uint8_t * BitStream, uint32_t Position);

// Raw pixel operations - no transformations, direct memory access
// These are fast inline functions for cases where you need direct pixel access

// Put pixel operations (write)
static inline void egfx_pp_1bpp(uint8_t *data, uint16_t size_x, uint16_t x, uint16_t y, egfx_pixel_state ps)
{
    uint32_t mem_width_in_bytes = EGFX_CALC_1BPP_BUFFER_ROW_BYTE_SIZE(size_x);
    uint32_t offset = (y * mem_width_in_bytes) + (x >> 3);
    uint8_t mask = 0x01 << (x & 0x07);
    if(ps) {
        data[offset] |= mask;
    } else {
        data[offset] &= ~mask;
    }
}

static inline void egfx_pp_3bpp(uint8_t *data, uint16_t size_x, uint16_t x, uint16_t y, egfx_pixel_state ps)
{
    uint32_t bit_stream_position = x * 3 + y * (size_x * 3);
    egfx_bit_stream_put(data, bit_stream_position++, ps & 0x01);
    egfx_bit_stream_put(data, bit_stream_position++, ps & 0x02);
    egfx_bit_stream_put(data, bit_stream_position, ps & 0x04);
}

static inline void egfx_pp_4bpp(uint8_t *data, uint16_t size_x, uint16_t x, uint16_t y, egfx_pixel_state ps)
{
    uint32_t mem_width_in_bytes = EGFX_CALC_4BPP_BUFFER_ROW_BYTE_SIZE(size_x);
    uint32_t offset = (y * mem_width_in_bytes) + (x >> 1);
    if(x & 0x01) {
        data[offset] &= ~0x0F;
        data[offset] |= (uint8_t)(ps & 0xF);
    } else {
        data[offset] &= ~0xF0;
        data[offset] |= (uint8_t)((ps & 0xF) << 4);
    }
}

static inline void egfx_pp_8bpp(uint8_t *data, uint16_t size_x, uint16_t x, uint16_t y, egfx_pixel_state ps)
{
    uint32_t offset = (y * size_x) + x;
    data[offset] = (uint8_t)ps;
}

static inline void egfx_pp_16bpp(uint8_t *data, uint16_t size_x, uint16_t x, uint16_t y, egfx_pixel_state ps)
{
    uint32_t mem_width_in_bytes = size_x << 1;
    uint32_t offset = (y * mem_width_in_bytes) + (x << 1);
    *(uint16_t *)(data + offset) = (uint16_t)ps;
}

static inline void egfx_pp_24bpp(uint8_t *data, uint16_t size_x, uint16_t x, uint16_t y, egfx_pixel_state ps)
{
    uint32_t mem_width_in_bytes = size_x * 3;
    uint32_t offset = (y * mem_width_in_bytes) + (x * 3);
    memcpy(&data[offset], &ps, 3);
}

static inline void egfx_pp_32bpp(uint8_t *data, uint16_t size_x, uint16_t x, uint16_t y, egfx_pixel_state ps)
{
    ((uint32_t *)data)[y * size_x + x] = (uint32_t)ps;
}

// Get pixel operations (read)
static inline egfx_pixel_state egfx_gp_1bpp(const uint8_t *data, uint16_t size_x, uint16_t x, uint16_t y)
{
    uint32_t mem_width_in_bytes = EGFX_CALC_1BPP_BUFFER_ROW_BYTE_SIZE(size_x);
    uint32_t offset = (y * mem_width_in_bytes) + (x >> 3);
    uint8_t mask = 0x01 << (x & 0x07);
    return (data[offset] & mask) ? EGFX_PIXEL_ON : EGFX_PIXEL_OFF;
}

static inline egfx_pixel_state egfx_gp_3bpp(const uint8_t *data, uint16_t size_x, uint16_t x, uint16_t y)
{
    uint32_t bit_stream_position = x * 3 + y * size_x * 3;
    egfx_pixel_state ps = 0;
    if(egfx_bit_stream_get((uint8_t *)data, bit_stream_position++))
        ps |= 0x01;
    if(egfx_bit_stream_get((uint8_t *)data, bit_stream_position++))
        ps |= 0x02;
    if(egfx_bit_stream_get((uint8_t *)data, bit_stream_position++))
        ps |= 0x04;
    return ps;
}

static inline egfx_pixel_state egfx_gp_4bpp(const uint8_t *data, uint16_t size_x, uint16_t x, uint16_t y)
{
    uint32_t mem_width_in_bytes = EGFX_CALC_4BPP_BUFFER_ROW_BYTE_SIZE(size_x);
    uint32_t offset = (y * mem_width_in_bytes) + (x >> 1);
    if(x & 0x01) {
        return data[offset] & 0x0F;
    } else {
        return (data[offset] >> 4) & 0x0F;
    }
}

static inline egfx_pixel_state egfx_gp_8bpp(const uint8_t *data, uint16_t size_x, uint16_t x, uint16_t y)
{
    uint32_t offset = (y * size_x) + x;
    return data[offset];
}

static inline egfx_pixel_state egfx_gp_16bpp(const uint8_t *data, uint16_t size_x, uint16_t x, uint16_t y)
{
    uint32_t mem_width_in_bytes = size_x << 1;
    uint32_t offset = (y * mem_width_in_bytes) + (x << 1);
    return *(uint16_t *)(data + offset);
}

static inline egfx_pixel_state egfx_gp_24bpp(const uint8_t *data, uint16_t size_x, uint16_t x, uint16_t y)
{
    uint32_t mem_width_in_bytes = size_x * 3;
    uint32_t offset = (y * mem_width_in_bytes) + (x * 3);
    egfx_pixel_state ps;
    ps  = (int32_t)data[offset];
    ps |= ((int32_t)data[offset + 1]) << 8;
    ps |= ((int32_t)data[offset + 2]) << 16;
    return ps;
}

static inline egfx_pixel_state egfx_gp_32bpp(const uint8_t *data, uint16_t size_x, uint16_t x, uint16_t y)
{
    return ((uint32_t *)data)[y * size_x + x];
}

// Function pointer helpers
egfx_put_pixel_t* egfx_get_put_pixel_func(const egfx_img* Image);

void EGFX_WEAK egfx_put_pixel(const egfx_img *Image,
                             uint16_t x,
                             uint16_t y,
                             egfx_pixel_state ps);

void EGFX_WEAK egfx_put_pixel_stub(const egfx_img *Image,
                                  uint16_t x,
                                  uint16_t y,
                                  egfx_pixel_state ps);

void EGFX_WEAK egfx_put_pixel_1bpp(const egfx_img *Image,
                                  uint16_t x,
                                  uint16_t y,
                                  egfx_pixel_state ps);

void EGFX_WEAK egfx_put_pixel_3bpp(const egfx_img *Image,
                                  uint16_t x,
                                  uint16_t y,
                                  egfx_pixel_state ps);

void EGFX_WEAK egfx_put_pixel_4bpp(const egfx_img *Image,
                                  uint16_t x,
                                  uint16_t y,
                                  egfx_pixel_state ps);

void EGFX_WEAK egfx_put_pixel_8bpp(const egfx_img *Image,
                                  uint16_t x,
                                  uint16_t y,
                                  egfx_pixel_state ps);

void EGFX_WEAK egfx_put_pixel_16bpp(const egfx_img *Image,
                                  uint16_t x,
                                  uint16_t y,
                                  egfx_pixel_state ps);

void EGFX_WEAK egfx_put_pixel_24bpp(const egfx_img *Image,
                                  uint16_t x,
                                  uint16_t y,
                                  egfx_pixel_state ps);

void EGFX_WEAK egfx_put_pixel_32bpp(const egfx_img *Image,
                                  uint16_t x,
                                  uint16_t y,
                                  egfx_pixel_state ps);

egfx_get_pixel_t *egfx_get_get_pixel_func(const egfx_img *IP);

egfx_pixel_state EGFX_WEAK egfx_get_pixel(const egfx_img *Image,
                                        uint16_t x,
                                        uint16_t y
);

egfx_pixel_state EGFX_WEAK egfx_get_pixel_stub(const egfx_img *Image,
                                  uint16_t x,
                                  uint16_t y
                                  );

egfx_pixel_state EGFX_WEAK egfx_get_pixel_1bpp(const egfx_img *Image,
                                  uint16_t x,
                                  uint16_t y
                                  );

egfx_pixel_state EGFX_WEAK egfx_get_pixel_3bpp(const egfx_img *Image,
                                  uint16_t x,
                                  uint16_t y
                                  );

egfx_pixel_state EGFX_WEAK egfx_get_pixel_4bpp(const egfx_img *Image,
                                  uint16_t x,
                                  uint16_t y
                                  );

egfx_pixel_state EGFX_WEAK egfx_get_pixel_8bpp(const egfx_img *Image,
                                  uint16_t x,
                                  uint16_t y
                                  );

egfx_pixel_state EGFX_WEAK egfx_get_pixel_16bpp(const egfx_img *Image,
                                  uint16_t x,
                                  uint16_t y
                                  );

egfx_pixel_state EGFX_WEAK egfx_get_pixel_24bpp(const egfx_img *Image,
                                  uint16_t x,
                                  uint16_t y
                                  );

egfx_pixel_state EGFX_WEAK egfx_get_pixel_32bpp(const egfx_img *Image,
                                  uint16_t x,
                                  uint16_t y
                                  );                                         


#endif

#ifdef __cplusplus
 }
#endif
