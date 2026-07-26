# eGFX Zephyr Test - 320x172 16BPP RGB565

This is a Zephyr RTOS application demonstrating the eGFX graphics library integration with Zephyr's native display driver.

## Configuration

- Target Board: native_sim (Zephyr simulator)
- Display: 320x172 pixels, SDL-based
- Pixel Format: 16BPP RGB565
- Display Driver: Zephyr SDL display driver

## Features

- Integrates eGFX with Zephyr's display subsystem
- Uses Zephyr display API (`display_write`) instead of eGFX dump functions
- SDL-based display window for native_sim testing
- Real-time frame counter demonstration

## Prerequisites

1. Zephyr SDK installed (v3.5.0 or later)
2. West workspace initialized
3. eGFX registered as a Zephyr module
4. SDL2 development libraries (for native_sim display)

### Installing SDL2 (for native_sim)

**Ubuntu/Debian:**
```bash
sudo apt-get install libsdl2-dev
```

**Windows (MSYS2):**
```bash
pacman -S mingw-w64-x86_64-SDL2
```

**macOS:**
```bash
brew install sdl2
```

## Building

Build for native_sim board:

```bash
cd egfx/test/sim-zephyr/prj/test_320_172_16BPP_RGB565
west build -b native_sim
```

Clean and rebuild:

```bash
west build -b native_sim -p
```

## Running

Run the native_sim application (opens SDL window):

```bash
west build -t run
```

The application will:
1. Initialize Zephyr display driver
2. Create eGFX back buffer
3. Render text and graphics
4. Display frame counter at 60 FPS

## Configuration Options

The application can be configured via Kconfig. Key options:

- `CONFIG_EGFX`: Enable eGFX library (set in prj.conf)
- `CONFIG_EGFX_PHYSICAL_SCREEN_SIZE_X`: Screen width (default: 320)
- `CONFIG_EGFX_PHYSICAL_SCREEN_SIZE_Y`: Screen height (default: 172)
- `CONFIG_EGFX_DISPLAY_DRIVER_IMG_TYPE_16BPP_RGB565`: Pixel format

To modify configuration:

```bash
west build -t menuconfig
```

Navigate to: **Modules → egfx**

## Integrating eGFX with Zephyr

To use eGFX in your own Zephyr project:

1. Add eGFX to your west manifest (`west.yml`):

```yaml
manifest:
  projects:
    - name: egfx
      url: <your-egfx-repo-url>
      revision: main
      path: modules/lib/egfx
```

2. Enable eGFX in your `prj.conf`:

```
CONFIG_EGFX=y
CONFIG_EGFX_PHYSICAL_SCREEN_SIZE_X=320
CONFIG_EGFX_PHYSICAL_SCREEN_SIZE_Y=172
```

3. Include eGFX headers in your application:

```c
#include "egfx.h"
```

## Display Driver Integration

This example uses Zephyr's display subsystem directly:

- **Native Sim**: SDL display driver (CONFIG_SDL_DISPLAY)
- **Hardware**: Use device tree to configure your display controller

The integration approach:

1. **Initialize eGFX buffer**: Create back buffer using `egfx_image_plane_init()`
2. **Render with eGFX**: Use eGFX drawing functions (fill, text, primitives)
3. **Display with Zephyr**: Use `display_write()` to push buffer to screen

### Porting to Hardware

To port this example to a hardware board with a display:

1. **Update board configuration**: Create/modify board overlay with display node
2. **Configure display driver**: Enable appropriate display driver in `prj.conf`
3. **Adjust pixel format**: Match eGFX format to display capabilities
4. **Optional**: Add DMA or other optimizations

Example device tree overlay for hardware:

```dts
&display_controller {
    status = "okay";
    width = <320>;
    height = <172>;
    pixel-format = <PANEL_PIXEL_FORMAT_RGB_565>;
};
```

## Architecture

```
┌─────────────────┐
│ Application     │  Uses eGFX API for drawing
│   (main.c)      │
└────────┬────────┘
         │
         ▼
┌─────────────────┐
│ eGFX Library    │  Rendering engine (text, primitives, etc.)
│                 │
└────────┬────────┘
         │ Writes to
         ▼
┌─────────────────┐
│ Frame Buffer    │  16BPP RGB565 buffer in RAM
│  (egfx_img)     │
└────────┬────────┘
         │ display_write()
         ▼
┌─────────────────┐
│ Zephyr Display  │  SDL (native_sim) or hardware driver
│     Driver      │
└─────────────────┘
```
