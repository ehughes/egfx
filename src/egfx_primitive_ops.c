
#include "egfx_config.h"
#include "egfx_types.h"
#include "egfx_pixel_ops.h"
#include "egfx_primitive_ops.h"

void EGFX_WEAK egfx_draw_h_line(egfx_img *image,
                                int32_t x_start,
                                int32_t x_stop,
                                int32_t y,
                                egfx_pixel_state ps)
{
    int32_t line_start;
    int32_t line_stop;
    int i;

    if ((y < image->size_y) && (y >= 0))
    {
        if (x_start > x_stop)
        {
            line_start = x_stop;
            line_stop = x_start;
        }
        else
        {
            line_start = x_start;
            line_stop = x_stop;
        }

        if (line_start < 0)
        {
            line_start = 0;
        }

        if (line_stop > image->size_x)
        {
            line_stop = image->size_x - 1;
        }

        if (line_start == line_stop)
        {
            egfx_put_pixel(image, line_start, y, ps);
        }
        else
        {
            for (i = line_start; i <= line_stop; i++)
            {
                egfx_put_pixel(image, i, y, ps);
            }
        }
    }
}

void EGFX_WEAK egfx_draw_v_line(egfx_img *image,
                                int32_t y_start,
                                int32_t y_stop,
                                int32_t x,
                                egfx_pixel_state ps)
{
    int32_t line_start;
    int32_t line_stop;
    int32_t i;

    if ((x < image->size_x) && (x >= 0))
    {
        if (y_start > y_stop)
        {
            line_start = y_stop;
            line_stop = y_start;
        }
        else
        {
            line_start = y_start;
            line_stop = y_stop;
        }

        if (line_start < 0)
        {
            line_start = 0;
        }

        if (line_stop > image->size_y)
        {
            line_stop = image->size_y - 1;
        }

        for (i = line_start; i <= line_stop; i++)
        {
            egfx_put_pixel(image, x, i, ps);
        }
    }
}

void EGFX_WEAK egfx_draw_filled_box(egfx_img *image,
                                    egfx_box *box,
                                    egfx_pixel_state ps)
{
    int32_t i;

    for (i = box->p1.y; i < box->p2.y + 1; i++)
    {
        egfx_draw_h_line(image, box->p1.x, box->p2.x, i, ps);
    }
}


void EGFX_WEAK egfx_draw_box(egfx_img *image,
                             egfx_box *box,
                             egfx_pixel_state ps)
{
    egfx_draw_h_line(image, box->p1.x, box->p2.x, box->p1.y, ps);
    egfx_draw_h_line(image, box->p1.x, box->p2.x, box->p2.y, ps);
    egfx_draw_v_line(image, box->p1.y, box->p2.y, box->p1.x, ps);
    egfx_draw_v_line(image, box->p1.y, box->p2.y, box->p2.x, ps);
}

void EGFX_WEAK egfx_draw_solid_rectangle(egfx_img *image,
                                         egfx_rect *r,
                                         egfx_pixel_state ps)
{
    for (int i = r->p1.y; i < r->p2.y + 1; i++)
    {
        egfx_draw_h_line(image, r->p1.x, r->p2.x, i, ps);
    }
}


void EGFX_WEAK egfx_draw_line(egfx_img *image,
                              int32_t x1,
                              int32_t y1,
                              int32_t x2,
                              int32_t y2,
                              egfx_pixel_state ps)
{
    // A simple Implementation of Bresenham's line Algorithm
    int32_t start_x, stop_x, start_y, stop_y;
    int32_t dx, dy;
    int32_t y_numerator;
    int32_t x_numerator;
    int32_t y;
    int32_t x;
    int32_t i;
    uint32_t y_dir = 0;

    // First Make sure that it is left to right
    // If not them flop them
    if (x2 > x1)
    {
        start_x = x1;
        stop_x = x2;
        start_y = y1;
        stop_y = y2;
    }
    else
    {
        start_x = x2;
        stop_x = x1;
        start_y = y2;
        stop_y = y1;
    }

    egfx_put_pixel(image, stop_x, stop_y, ps);

    if (stop_y >= start_y)
    {
        dy = stop_y - start_y;
        y_dir = 0;
    }
    else
    {
        dy = start_y - stop_y;
        y_dir = 1;
    }

    dx = stop_x - start_x;

    // Now, if the slope is less greater than one, we need to swap all X/Y operations
    if (dy <= dx)
    {
        // Slope is less than one, proceed at normal and step along the x axis
        y = start_y;   // start the whole part of the Y value at the starting pixel.
        x = start_x;
        // We need to start the numerator of the fraction half way through the fraction so everything rounds at
        // fraction midpoint
        y_numerator = dx >> 1;   // The fraction denominator is assumed to be dx

        // our fixed point Y value is  Y + (y_numerator / dx)
        // Every time we step the X coordinate by one, we need to step
        // our Y coordinate by dy/dx.  We do this by just adding dy to our
        // numerator.  When the numerator gets bigger than the
        // denominator, the increment the whole part by one and decrement the numerator
        // by the denominator
        for (i = 0; i < dx; i++)
        {
            egfx_put_pixel(image, x, y, ps);
            x++;
            // Now do all the fractional stuff
            y_numerator += dy;

            if (y_numerator >= dx)
            {
                y_numerator -= dx;

                if (stop_y > start_y)
                {
                    y++;
                }
                else
                {
                    y--;
                }
            }
        }
    }
    else
    {
        // Same as before by step along the y axis.
        y = start_y;
        x = start_x;
        x_numerator = dy >> 1;

        for (i = 0; i < dy; i++)
        {
            egfx_put_pixel(image, x, y, ps);

            // Now do all the fractional stuff
            if (y_dir)
            {
                y--;
            }
            else
            {
                y++;
            }

            x_numerator += dx;

            if (x_numerator >= dy)
            {
                x_numerator -= dy;

                if (stop_x > start_x)
                {
                    x++;
                }
                else
                {
                    x--;
                }
            }
        }
    }
}

// midpoint circle algorithm
void EGFX_WEAK egfx_draw_circle(egfx_img *image,
                                int32_t x0,
                                int32_t y0,
                                int32_t radius,
                                egfx_pixel_state ps)
{
    int32_t f = 1 - radius;
    int32_t dd_f_x = 1;
    int32_t dd_f_y = -2 * radius;
    int32_t x = 0;
    int32_t y = radius;

    if (radius <= 0)
        return;

    egfx_put_pixel(image, x0, y0 + radius, ps);
    egfx_put_pixel(image, x0, y0 - radius, ps);
    egfx_put_pixel(image, x0 + radius, y0, ps);
    egfx_put_pixel(image, x0 - radius, y0, ps);

    while (x < y)
    {
        // dd_f_x == 2 * x + 1;
        // dd_f_y == -2 * y;
        // f == x*x + y*y - radius*radius + 2*x - y + 1;
        if (f >= 0)
        {
            y--;
            dd_f_y += 2;
            f += dd_f_y;
        }

        x++;
        dd_f_x += 2;
        f += dd_f_x;
        egfx_put_pixel(image, x0 + x, y0 + y, ps);
        egfx_put_pixel(image, x0 - x, y0 + y, ps);
        egfx_put_pixel(image, x0 + x, y0 - y, ps);
        egfx_put_pixel(image, x0 - x, y0 - y, ps);
        egfx_put_pixel(image, x0 + y, y0 + x, ps);
        egfx_put_pixel(image, x0 - y, y0 + x, ps);
        egfx_put_pixel(image, x0 + y, y0 - x, ps);
        egfx_put_pixel(image, x0 - y, y0 - x, ps);
    }
}

// midpoint circle algorithm
void EGFX_WEAK egfx_draw_circle_from_background(egfx_img *image,
                                                egfx_img *background,
                                                int32_t x0,
                                                int32_t y0,
                                                int32_t radius)
{
    int32_t f = 1 - radius;
    int32_t dd_f_x = 1;
    int32_t dd_f_y = -2 * radius;
    int32_t x = 0;
    int32_t y = radius;

    if (radius <= 0)
        return;

    egfx_put_pixel(image, x0, y0 + radius, egfx_get_pixel(background, x0, y0 + radius));
    egfx_put_pixel(image, x0, y0 - radius, egfx_get_pixel(background, x0, y0 - radius));
    egfx_put_pixel(image, x0 + radius, y0, egfx_get_pixel(background, x0 + radius, y0));
    egfx_put_pixel(image, x0 - radius, y0, egfx_get_pixel(background, x0 - radius, y0));

    while (x < y)
    {
        // dd_f_x == 2 * x + 1;
        // dd_f_y == -2 * y;
        // f == x*x + y*y - radius*radius + 2*x - y + 1;
        if (f >= 0)
        {
            y--;
            dd_f_y += 2;
            f += dd_f_y;
        }

        x++;
        dd_f_x += 2;
        f += dd_f_x;
        egfx_put_pixel(image, x0 + x, y0 + y, egfx_get_pixel(background, x0 + x, y0 + y));
        egfx_put_pixel(image, x0 - x, y0 + y, egfx_get_pixel(background, x0 - x, y0 + y));
        egfx_put_pixel(image, x0 + x, y0 - y, egfx_get_pixel(background, x0 + x, y0 - y));
        egfx_put_pixel(image, x0 - x, y0 - y, egfx_get_pixel(background, x0 - x, y0 - y));
        egfx_put_pixel(image, x0 + y, y0 + x, egfx_get_pixel(background, x0 + y, y0 + x));
        egfx_put_pixel(image, x0 - y, y0 + x, egfx_get_pixel(background, x0 - y, y0 + x));
        egfx_put_pixel(image, x0 + y, y0 - x, egfx_get_pixel(background, x0 + y, y0 - x));
        egfx_put_pixel(image, x0 - y, y0 - x, egfx_get_pixel(background, x0 - y, y0 - x));
    }
}

void EGFX_WEAK egfx_draw_filled_circle(egfx_img *image,
                                       int16_t x0,
                                       int16_t y0,
                                       int16_t radius,
                                       egfx_pixel_state ps)
{
    int16_t f = 1 - radius;
    int16_t dd_f_x = 1;
    int16_t dd_f_y = -2 * radius;
    int16_t x = 0;
    int16_t y = radius;

    if (radius == 0)
        return;

    egfx_draw_h_line(image, x0 - radius, x0 + radius, y0, ps);

    egfx_draw_v_line(image, y0 - radius, y0 + radius, x0, ps);

    while (x < y)
    {
        // dd_f_x == 2 * x + 1;
        // dd_f_y == -2 * y;
        // f == x*x + y*y - radius*radius + 2*x - y + 1;
        if (f >= 0)
        {
            y--;
            dd_f_y += 2;
            f += dd_f_y;
        }

        x++;
        dd_f_x += 2;
        f += dd_f_x;

        egfx_draw_h_line(image, x0 - x, x0 + x, y0 + y, ps);

        egfx_draw_h_line(image, x0 - x, x0 + x, y0 - y, ps);

        egfx_draw_h_line(image, x0 - y, x0 + y, y0 + x, ps);

        egfx_draw_h_line(image, x0 - y, x0 + y, y0 - x, ps);
    }
}
