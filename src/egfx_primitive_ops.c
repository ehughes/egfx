
#include "egfx_config.h"
#include "egfx_types.h"
#include "egfx_pixel_ops.h"
#include "egfx_primitive_ops.h"

void EGFX_WEAK eGFX_DrawH_Line(egfx_img *Image,
                               int32_t X_Start,
                               int32_t X_Stop,
                               int32_t Y,
                               egfx_pixel_state PS)
{
    int32_t LineStart;
    int32_t LineStop;
    int i;

    if((Y<Image->size_y)  && (Y>=0))
    {
        if(X_Start>X_Stop)
        {
            LineStart = X_Stop;
            LineStop = X_Start;
        }
        else
        {
            LineStart = X_Start;
            LineStop = X_Stop;
        }

        if(LineStart<0)
        {
            LineStart = 0;
        }

        if(LineStop>Image->size_x)
        {
            LineStop =  Image->size_x-1;
        }

        if(LineStart == LineStop)
        {
            egfx_put_pixel(Image,LineStart,Y,PS);
        }
        else
        {
            for(i=LineStart; i<=LineStop ; i++)
            {
                egfx_put_pixel(Image,i,Y,PS);
            }
        }
    }
}

void EGFX_WEAK eGFX_DrawV_Line(egfx_img *Image,
                              int32_t YStart,
                              int32_t YStop,
                              int32_t X,
                              egfx_pixel_state PS)
{
    int32_t LineStart;
    int32_t LineStop;
    int32_t i;

    if((X<Image->size_x) && (X>=0))
    {
        if(YStart>YStop)
        {
            LineStart = YStop;
            LineStop = YStart;
        }
        else
        {
            LineStart = YStart;
            LineStop = YStop;
        }

        if(LineStart<0)
        {
            LineStart = 0;
        }

        if(LineStop>Image->size_y)
        {
            LineStop =  Image->size_y-1;
        }

        for(i=LineStart; i<=LineStop ; i++)
        {
            egfx_put_pixel(Image,X,i,PS);
        }
    }
}

void EGFX_WEAK eGFX_DrawFilledBox(egfx_img *Image,
                                  egfx_box *Box,
                                  egfx_pixel_state PS)
{
    int32_t i;

    for(i=Box->p1.y ; i<Box->p2.y+1 ; i++)
    {
        eGFX_DrawH_Line(Image, Box->p1.x, Box->p2.x,i,PS);
    }
}


void EGFX_WEAK eGFX_DrawBox(egfx_img *Image,
                            egfx_box *Box,
                            egfx_pixel_state PS)
{
    eGFX_DrawH_Line(Image, Box->p1.x, Box->p2.x,Box->p1.y,PS);
    eGFX_DrawH_Line(Image, Box->p1.x, Box->p2.x,Box->p2.y,PS);
    eGFX_DrawV_Line(Image, Box->p1.y, Box->p2.y,Box->p1.x,PS);
    eGFX_DrawV_Line(Image, Box->p1.y, Box->p2.y,Box->p2.x,PS);
}

void EGFX_WEAK eGFX_DrawSolidRectangle(egfx_img *Image,
	egfx_rect * R,
	egfx_pixel_state PS)
{
	for (int i = R->p1.y; i < R->p2.y + 1; i++)
	{
		eGFX_DrawH_Line(Image, R->p1.x, R->p2.x, i, PS);
	}
}


void EGFX_WEAK eGFX_DrawLine(egfx_img *Image,
                             int32_t X1,
                             int32_t Y1,
                             int32_t X2,
                             int32_t Y2,
                             egfx_pixel_state PS)
{
    //A simple Implementation of Bresenham's line Algorithm
    int32_t StartX,StopX,StartY,StopY;
    int32_t dX,dY;
    int32_t Y_Numerator;
    int32_t X_Numerator;
    int32_t Y;
    int32_t X;
    int32_t i;
    uint32_t YDir = 0;

    //First Make sure that it is left to right
    //If not them flop them
    if(X2>X1)
    {
        StartX = X1;
        StopX = X2;
        StartY = Y1;
        StopY = Y2;
    }
    else
    {
        StartX = X2;
        StopX = X1;
        StartY = Y2;
        StopY = Y1;
    }

    egfx_put_pixel(Image, StopX,StopY, PS);

    if(StopY>=StartY)
    {
        dY = StopY - StartY;
        YDir = 0;
    }
    else
    {
        dY = StartY - StopY;
        YDir = 1;
    }

    dX = StopX - StartX;

    //Now, if the slope is less greater than one,  we need to swap all X/Y operations
    if(dY<=dX)
    {
        //Slope is less than one, proceed at normal and step along the x axis
        Y=StartY;   //start the whole part of the Y value at the starting pixeel.
        X=StartX;
        //We need to start the numerator of the fraction half way through the fraction so evertyhing rounds at
        //fraction midpoint
        Y_Numerator = dX>>1;   //The fraction demonimator is assumeed to be dX

        // out fixed point Y value is  Y + (Y_Numerator / dX)
        //Every time we step the X coordinate by one, we need to step
        //out Y coordinate by dY/dX.  We do this by just adding dY to our
        //numerator.  When the numerator gets bigger than the
        //denomiator, the increment the whole part by one and decrement the numerator
        //by the denominator
        for(i=0; i<dX; i++)
        {
            egfx_put_pixel(Image,X,Y,PS);
            X++;
            //Now do all the fractional stuff
            Y_Numerator += dY;

            if(Y_Numerator >= dX)
            {
                Y_Numerator-=dX;

                if(StopY > StartY)
                {
                    Y++;
                }
                else
                {
                    Y--;
                }
            }
        }
    }
    else
    {
        //Same as before by step along the y axis.
        Y=StartY;
        X=StartX;
        X_Numerator = dY>>1;

        for(i=0; i<dY; i++)
        {
            egfx_put_pixel(Image,X,Y,PS);

            //Now do all the fractional stuff
            if(YDir)
            {
                Y--;
            }
            else
            {
                Y++;
            }

            X_Numerator += dX;

            if(X_Numerator >= dY)
            {
                X_Numerator-=dY;

                if(StopX > StartX)
                {
                    X++;
                }
                else
                {
                    X--;
                }
            }
        }
    }
}

// midpoint circle algorithm
void EGFX_WEAK eGFX_DrawCircle(egfx_img *Image,
                               int32_t x0,
                               int32_t y0,
                               int32_t radius,
                               egfx_pixel_state PS)
{
    int32_t f = 1 - radius;
    int32_t ddF_x = 1;
    int32_t ddF_y = -2 * radius;
    int32_t x = 0;
    int32_t y = radius;

        if (radius <= 0)
                return;

    egfx_put_pixel(Image, x0, y0 + radius, PS);
    egfx_put_pixel(Image, x0, y0 - radius, PS);
    egfx_put_pixel(Image, x0 + radius, y0, PS);
    egfx_put_pixel(Image, x0 - radius, y0, PS);


    while(x < y)
    {
        // ddF_x == 2 * x + 1;
        // ddF_y == -2 * y;
        // f == x*x + y*y - radius*radius + 2*x - y + 1;
        if(f >= 0)
        {
            y--;
            ddF_y += 2;
            f += ddF_y;
        }

        x++;
        ddF_x += 2;
        f += ddF_x;
        egfx_put_pixel(Image, x0 + x, y0 + y, PS);
        egfx_put_pixel(Image, x0 - x, y0 + y, PS);
        egfx_put_pixel(Image, x0 + x, y0 - y, PS);
        egfx_put_pixel(Image, x0 - x, y0 - y, PS);
        egfx_put_pixel(Image, x0 + y, y0 + x, PS);
        egfx_put_pixel(Image, x0 - y, y0 + x, PS);
        egfx_put_pixel(Image, x0 + y, y0 - x, PS);
        egfx_put_pixel(Image, x0 - y, y0 - x, PS);
    }
}

// midpoint circle algorithm
void EGFX_WEAK eGFX_DrawCircleFromBackground(egfx_img *Image,
	egfx_img *Background,
	int32_t x0,
	int32_t y0,
	int32_t radius)
{
	int32_t f = 1 - radius;
	int32_t ddF_x = 1;
	int32_t ddF_y = -2 * radius;
	int32_t x = 0;
	int32_t y = radius;

	if (radius <= 0)
		return;

	
	egfx_put_pixel(Image, x0, y0 + radius, egfx_get_pixel(Background, x0, y0 + radius));
	egfx_put_pixel(Image, x0, y0 - radius, egfx_get_pixel(Background, x0, y0 - radius));
	egfx_put_pixel(Image, x0 + radius, y0, egfx_get_pixel(Background, x0 + radius, y0));
	egfx_put_pixel(Image, x0 - radius, y0, egfx_get_pixel(Background, x0 - radius, y0));


	while (x < y)
	{
		// ddF_x == 2 * x + 1;
		// ddF_y == -2 * y;
		// f == x*x + y*y - radius*radius + 2*x - y + 1;
		if (f >= 0)
		{
			y--;
			ddF_y += 2;
			f += ddF_y;
		}

		x++;
		ddF_x += 2;
		f += ddF_x;
		egfx_put_pixel(Image, x0 + x, y0 + y, egfx_get_pixel(Background, x0 + x, y0 + y));
		egfx_put_pixel(Image, x0 - x, y0 + y, egfx_get_pixel(Background, x0 - x, y0 + y));
		egfx_put_pixel(Image, x0 + x, y0 - y, egfx_get_pixel(Background, x0 + x, y0 - y));
		egfx_put_pixel(Image, x0 - x, y0 - y, egfx_get_pixel(Background, x0 - x, y0 - y));
		egfx_put_pixel(Image, x0 + y, y0 + x, egfx_get_pixel(Background, x0 + y, y0 + x));
		egfx_put_pixel(Image, x0 - y, y0 + x, egfx_get_pixel(Background, x0 - y, y0 + x));
		egfx_put_pixel(Image, x0 + y, y0 - x, egfx_get_pixel(Background, x0 + y, y0 - x));
		egfx_put_pixel(Image, x0 - y, y0 - x, egfx_get_pixel(Background, x0 - y, y0 - x));
	}
}

void EGFX_WEAK eGFX_DrawFilledCircle(egfx_img *Image,
                                    int16_t x0,
                                    int16_t y0,
                                    int16_t radius,
                                    egfx_pixel_state PS)
{
        int16_t f = 1 - radius;
        int16_t ddF_x = 1;
        int16_t ddF_y = -2 * radius;
        int16_t x = 0;
        int16_t y = radius;

        if (radius == 0)
                return;

        eGFX_DrawH_Line(Image, x0 - radius, x0 + radius , y0, PS);

        eGFX_DrawV_Line(Image, y0 - radius , y0 + radius , x0, PS);

        while (x < y)
        {
                // ddF_x == 2 * x + 1;
                // ddF_y == -2 * y;
                // f == x*x + y*y - radius*radius + 2*x - y + 1;
                if (f >= 0)
                {
                        y--;
                        ddF_y += 2;
                        f += ddF_y;
                }

                x++;
                ddF_x += 2;
                f += ddF_x;
                
                eGFX_DrawH_Line(Image, x0 - x, x0 + x, y0 + y, PS);

                eGFX_DrawH_Line(Image, x0 - x, x0 + x, y0 - y, PS);

                eGFX_DrawH_Line(Image, x0 - y, x0 + y, y0 + x, PS);

                eGFX_DrawH_Line(Image, x0 - y, x0 + y, y0 - x, PS);


        }
}
