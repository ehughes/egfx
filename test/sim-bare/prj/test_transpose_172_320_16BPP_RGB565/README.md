# Transpose Test - 172x320 Physical → 320x172 Logical

This test demonstrates the eGFX transpose functionality for displays with rotated memory layouts.

## Overview

Some displays organize their memory in a transposed orientation relative to how you want to address them. For example, a display that you want to use as 320 pixels wide × 172 pixels tall might have its internal memory organized as 172 pixels wide × 320 pixels tall.

This test program shows how to use the `EGFX_IMG_FLAGS_TRANSPOSE` flag to handle this situation transparently.

## Configuration

- **Physical dimensions**: 172 (width) × 320 (height) - how memory is organized
- **Logical dimensions**: 320 (width) × 172 (height) - how you address it in code
- **Pixel format**: 16BPP RGB565
- **Transform flag**: `EGFX_IMG_FLAGS_TRANSPOSE`

## How It Works

1. The display driver creates a framebuffer with physical dimensions (172×320)
2. The test code swaps the `size_x` and `size_y` fields to logical dimensions (320×172)
3. The test code sets `EGFX_IMG_FLAGS_TRANSPOSE` on the backbuffer
4. All pixel operations automatically transform coordinates:
   - Logical coordinate (x, y) → Physical coordinate (y, x)
   - The display window shows the physical orientation (172 wide × 320 tall)
   - Your code uses logical coordinates (320 wide × 172 tall)

## Code Example

```c
// After eGFX_InitDriver() creates backbuffers with physical dimensions
for (int i = 0; i < eGFX_NUM_BACKBUFFERS; i++) {
    // Swap dimensions to logical size
    uint16_t temp = eGFX_BackBuffer[i]->size_x;
    eGFX_BackBuffer[i]->size_x = eGFX_BackBuffer[i]->size_y;  // Set to 320
    eGFX_BackBuffer[i]->size_y = temp;                         // Set to 172

    // Enable transpose transformation
    eGFX_BackBuffer[i]->flags = EGFX_IMG_FLAGS_TRANSPOSE;
}

// Now use the backbuffer as if it's 320×172
egfx_text(eGFX_BackBuffer[0], "Hello World", (egfx_point){5, 5}, &config);
```

## Building and Running

```bash
cd test_transpose_172_320_16BPP_RGB565
cmake -S . -B build -G Ninja
cmake --build build
./build/egfx_test_transpose_172_320_16BPP_RGB565.exe
```

## Expected Result

- The display window will be 172 pixels wide × 320 pixels tall (portrait orientation)
- Text and graphics will appear rotated 90° clockwise in the window
- This demonstrates that logical coordinates are being correctly transformed to physical memory layout

## Comparison

Compare with `test_320_172_16BPP_RGB565`:
- **Normal test**: 320×172 physical = 320×172 logical (landscape)
- **Transpose test**: 172×320 physical = 320×172 logical (appears rotated)

Both tests use identical rendering code at logical 320×172 coordinates, but produce different visual orientations.
