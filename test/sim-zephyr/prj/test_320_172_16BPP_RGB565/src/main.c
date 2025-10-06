#include <zephyr/kernel.h>
#include <zephyr/device.h>
#include <zephyr/devicetree.h>
#include <zephyr/drivers/display.h>
#include <stdio.h>

#include "egfx.h"

// Get display device from devicetree
#define DISPLAY_NODE DT_CHOSEN(zephyr_display)

// eGFX back buffer storage
static uint8_t frame_buffer[eGFX_PHYSICAL_SCREEN_SIZE_X * eGFX_PHYSICAL_SCREEN_SIZE_Y * 2];
static egfx_img back_buffer;

int main(void)
{
	const struct device *display_dev;
	struct display_capabilities caps;
	struct display_buffer_descriptor buf_desc;
	int ret;

	printk("eGFX Test for Zephyr native_sim - 320x172 16BPP RGB565\n");

	// Get display device
	display_dev = DEVICE_DT_GET(DISPLAY_NODE);
	if (!device_is_ready(display_dev)) {
		printk("Display device not ready\n");
		return -1;
	}

	// Get display capabilities
	display_get_capabilities(display_dev, &caps);
	printk("Display: %dx%d, format: %d\n", caps.x_resolution, caps.y_resolution, caps.current_pixel_format);

	// Initialize eGFX back buffer
	egfx_image_plane_init(&back_buffer,
	                      frame_buffer,
	                      eGFX_PHYSICAL_SCREEN_SIZE_X,
	                      eGFX_PHYSICAL_SCREEN_SIZE_Y,
	                      eGFX_DISPLAY_DRIVER_IMG_TYPE);

	printk("eGFX buffer initialized: %dx%d\n", back_buffer.size_x, back_buffer.size_y);

	// Turn on display
	display_blanking_off(display_dev);

	// Setup buffer descriptor for display_write
	buf_desc.buf_size = eGFX_PHYSICAL_SCREEN_SIZE_X * eGFX_PHYSICAL_SCREEN_SIZE_Y * 2;
	buf_desc.width = eGFX_PHYSICAL_SCREEN_SIZE_X;
	buf_desc.height = eGFX_PHYSICAL_SCREEN_SIZE_Y;
	buf_desc.pitch = eGFX_PHYSICAL_SCREEN_SIZE_X;

	// Main rendering loop
	int frame = 0;
	while (1) {
		// Clear screen to white
		egfx_fill(&back_buffer, EGFX_COLOR_RGB888_TO_RGB565(255, 255, 255));

		// Draw some test content
		egfx_text(&back_buffer, "Zephyr eGFX Test", (egfx_point){5, 5},
			&(egfx_text_config){.font = &font_10_14, .colored = true, .color = {255, 0, 0}, .antialiased = false});

		char buf[32];
		snprintf(buf, sizeof(buf), "Frame: %d", frame++);
		egfx_text(&back_buffer, buf, (egfx_point){5, 25},
			&(egfx_text_config){.font = &font_5_7, .colored = true, .color = {0, 0, 255}, .antialiased = false});

		// Write buffer to display
		ret = display_write(display_dev, 0, 0, &buf_desc, frame_buffer);
		if (ret != 0) {
			printk("display_write failed: %d\n", ret);
		}

		// Delay for next frame (60 FPS = ~16ms)
		k_msleep(16);
	}

	return 0;
}
