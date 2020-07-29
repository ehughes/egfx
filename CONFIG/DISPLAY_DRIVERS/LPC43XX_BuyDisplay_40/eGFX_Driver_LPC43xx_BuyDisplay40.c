#ifdef CORE_M4
#include "eGFX.h"
#include "eGFX_Driver_LPC43xx_BuyDisplay40.h"

#include "Chip.h"
#include "board.h"
#include "System.h"


#define MINI_DELAY	for(volatile int i=0;i<500;i++){}

eGFX_ImagePlane eGFX_BackBuffer[eGFX_NUM_BACKBUFFERS];

__BSS(SDRAM) uint8_t eGFX_FrameBuffer[eGFX_NUM_BACKBUFFERS][eGFX_CALCULATE_16BPP_IMAGE_STORAGE_SPACE_SIZE(eGFX_PHYSICAL_SCREEN_SIZE_X,eGFX_PHYSICAL_SCREEN_SIZE_Y)];


uint8_t SPI_SendData(unsigned char i)
{
	LPC_SSP1->DR = i;

	while((LPC_SSP1->SR & (1<<4)) >0){}

	return LPC_SSP1->DR;

	return 0;
}

void SPI_WriteComm(unsigned char i)
{

	LCD_CS_ACTIVE; MINI_DELAY;
	LCD_DC_INACTIVE; MINI_DELAY;
	SPI_SendData(i); MINI_DELAY;
	LCD_CS_INACTIVE;MINI_DELAY
}


void SPI_WriteData(unsigned char i)
{

	LCD_CS_ACTIVE; MINI_DELAY;
	LCD_DC_ACTIVE; MINI_DELAY;
	SPI_SendData(i); MINI_DELAY;
	LCD_CS_INACTIVE;MINI_DELAY

}

void ST7701S_Initial(void)
{

	LCD_RESET_N_INACTIVE;
	System__Delay_mS(10);
	LCD_RESET_N_ACTIVE;
	System__Delay_mS(10);
	LCD_RESET_N_INACTIVE;
	System__Delay_mS(10);

	SPI_WriteComm(0x11);//sleep out
	System__Delay_mS(120); //Delay 120ms
	//--------------------------------Display and color format setting----------------------------//
	SPI_WriteComm (0x11); // sleep out
	System__Delay_mS(120);

	SPI_WriteComm (0xFF);
	SPI_WriteData (0x77);
	SPI_WriteData (0x01);
	SPI_WriteData (0x00);
	SPI_WriteData (0x00);
	SPI_WriteData (0x10);


	SPI_WriteComm (0xC0); //LNESET (C0h/C000h): Display Line Setting
	SPI_WriteData (0x3B);
	SPI_WriteData (0x00);

	SPI_WriteComm (0xC1); //PORCTRL (C1h/C100h):Porch Control
	SPI_WriteData (0x0D);
	SPI_WriteData (0x02);

	SPI_WriteComm (0xC2); //INVSET (C2h/C200h):Inversion selection & Frame Rate Control

	SPI_WriteData (0x21);
	SPI_WriteData (0x08); //512 + 8*16  Min num PCK each line   512+128 = 640

	SPI_WriteComm (0xB0); //PVGAMCTRL (B0h/B000h): Positive Voltage Gamma Control
	SPI_WriteData (0x00);
	SPI_WriteData (0x11);
	SPI_WriteData (0x18);
	SPI_WriteData (0x0E);
	SPI_WriteData (0x11);
	SPI_WriteData (0x06);
	SPI_WriteData (0x07);
	SPI_WriteData (0x08);
	SPI_WriteData (0x07);
	SPI_WriteData (0x22);
	SPI_WriteData (0x04);
	SPI_WriteData (0x12);
	SPI_WriteData (0x0F);
	SPI_WriteData (0xAA);
	SPI_WriteData (0x31);
	SPI_WriteData (0x18);

	SPI_WriteComm (0xB1); //NVGAMCTRL (B1h/B100h): Negative Voltage Gamma Control
	SPI_WriteData (0x00);
	SPI_WriteData (0x11);
	SPI_WriteData (0x19);
	SPI_WriteData (0x0E);
	SPI_WriteData (0x12);
	SPI_WriteData (0x07);
	SPI_WriteData (0x08);
	SPI_WriteData (0x08);
	SPI_WriteData (0x08);
	SPI_WriteData (0x22);
	SPI_WriteData (0x04);
	SPI_WriteData (0x11);
	SPI_WriteData (0x11);
	SPI_WriteData (0xA9);
	SPI_WriteData (0x32);
	SPI_WriteData (0x18);

	SPI_WriteComm (0xFF);  //Command2 BKx Selection
	SPI_WriteData (0x77);
	SPI_WriteData (0x01);
	SPI_WriteData (0x00);
	SPI_WriteData (0x00);
	SPI_WriteData (0x11);

	SPI_WriteComm (0xB0); //VRHS (B0h/B000h):Vop Amplitude setting
	SPI_WriteData (0x60);

	SPI_WriteComm (0xB1); //VCOMS (B1h/B100h):VCOM amplitude setting
	SPI_WriteData (0x30);

	SPI_WriteComm (0xB2); //12.3.3.3 VGHSS (B2h/B200h):VGH Voltage setting
	SPI_WriteData (0x87);

	SPI_WriteComm (0xB3);
	SPI_WriteData (0x80);

	SPI_WriteComm (0xB5); // VGLS (B5h/B500h):VGL Voltage setting
	SPI_WriteData (0x49);

	SPI_WriteComm (0xB7); //12.3.3.6 PWCTRL1 (B7h/B700h):Power Control 1
	SPI_WriteData (0x85);

	SPI_WriteComm (0xB8); //12.3.3.7 PWCTRL2 (B8h/B800h):Power Control 2
	SPI_WriteData (0x21);

	SPI_WriteComm (0xC1); //SPD1 (C1h/C100h): Source pre_drive timing set1
	SPI_WriteData (0x78);

	SPI_WriteComm (0xC2); //SPD2 (C2h/C200h):Source EQ2 Setting
	SPI_WriteData (0x78);

	System__Delay_mS(20);

	SPI_WriteComm (0xE0);// SECTRL (E0h/E000h):Sunlight Readable Enhancement
	SPI_WriteData (0x00);
	SPI_WriteData (0x1B);
	SPI_WriteData (0x02);

	SPI_WriteComm (0xE1);
	SPI_WriteData (0x08);
	SPI_WriteData (0xA0);
	SPI_WriteData (0x00);
	SPI_WriteData (0x00);
	SPI_WriteData (0x07);
	SPI_WriteData (0xA0);
	SPI_WriteData (0x00);
	SPI_WriteData (0x00);
	SPI_WriteData (0x00);
	SPI_WriteData (0x44);
	SPI_WriteData (0x44);

	SPI_WriteComm (0xE2);
	SPI_WriteData (0x11);
	SPI_WriteData (0x11);
	SPI_WriteData (0x44);
	SPI_WriteData (0x44);
	SPI_WriteData (0xED);
	SPI_WriteData (0xA0);
	SPI_WriteData (0x00);
	SPI_WriteData (0x00);
	SPI_WriteData (0xEC);
	SPI_WriteData (0xA0);
	SPI_WriteData (0x00);
	SPI_WriteData (0x00);

	SPI_WriteComm (0xE3);
	SPI_WriteData (0x00);
	SPI_WriteData (0x00);
	SPI_WriteData (0x11);
	SPI_WriteData (0x11);

	SPI_WriteComm (0xE4);
	SPI_WriteData (0x44);
	SPI_WriteData (0x44);

	SPI_WriteComm (0xE5);
	SPI_WriteData (0x0A);
	SPI_WriteData (0xE9);
	SPI_WriteData (0xD8);
	SPI_WriteData (0xA0);
	SPI_WriteData (0x0C);
	SPI_WriteData (0xEB);
	SPI_WriteData (0xD8);
	SPI_WriteData (0xA0);
	SPI_WriteData (0x0E);
	SPI_WriteData (0xED);
	SPI_WriteData (0xD8);
	SPI_WriteData (0xA0);
	SPI_WriteData (0x10);
	SPI_WriteData (0xEF);
	SPI_WriteData (0xD8);
	SPI_WriteData (0xA0);

	SPI_WriteComm (0xE6);
	SPI_WriteData (0x00);
	SPI_WriteData (0x00);
	SPI_WriteData (0x11);
	SPI_WriteData (0x11);

	SPI_WriteComm (0xE7);
	SPI_WriteData (0x44);
	SPI_WriteData (0x44);

	SPI_WriteComm (0xE8);
	SPI_WriteData (0x09);
	SPI_WriteData (0xE8);
	SPI_WriteData (0xD8);
	SPI_WriteData (0xA0);
	SPI_WriteData (0x0B);
	SPI_WriteData (0xEA);
	SPI_WriteData (0xD8);
	SPI_WriteData (0xA0);
	SPI_WriteData (0x0D);
	SPI_WriteData (0xEC);
	SPI_WriteData (0xD8);
	SPI_WriteData (0xA0);
	SPI_WriteData (0x0F);
	SPI_WriteData (0xEE);
	SPI_WriteData (0xD8);
	SPI_WriteData (0xA0);

	SPI_WriteComm (0xEB);
	SPI_WriteData (0x02);
	SPI_WriteData (0x00);
	SPI_WriteData (0xE4);
	SPI_WriteData (0xE4);
	SPI_WriteData (0x88);
	SPI_WriteData (0x00);
	SPI_WriteData (0x40);

	SPI_WriteComm (0xEC);
	SPI_WriteData (0x3C);
	SPI_WriteData (0x00);

	SPI_WriteComm (0xED);
	SPI_WriteData (0xAB);
	SPI_WriteData (0x89);
	SPI_WriteData (0x76);
	SPI_WriteData (0x54);
	SPI_WriteData (0x02);
	SPI_WriteData (0xFF);
	SPI_WriteData (0xFF);
	SPI_WriteData (0xFF);
	SPI_WriteData (0xFF);
	SPI_WriteData (0xFF);
	SPI_WriteData (0xFF);
	SPI_WriteData (0x20);
	SPI_WriteData (0x45);
	SPI_WriteData (0x67);
	SPI_WriteData (0x98);
	SPI_WriteData (0xBA);

	SPI_WriteComm (0xFF);
	SPI_WriteData (0x77);
	SPI_WriteData (0x01);
	SPI_WriteData (0x00);
	SPI_WriteData (0x00);
	SPI_WriteData (0x00);


	SPI_WriteComm (0x3A);
	SPI_WriteData (0x70);
	SPI_WriteComm (0x29);

}

void eGFX_InitDriver()
{

	for(int i=0;i<eGFX_NUM_BACKBUFFERS;i++)
	{

		eGFX_ImagePlaneInit(&eGFX_BackBuffer[i],
							&eGFX_FrameBuffer[i][0],
							eGFX_PHYSICAL_SCREEN_SIZE_X,
							eGFX_PHYSICAL_SCREEN_SIZE_Y,
							eGFX_IMAGE_PLANE_16BPP_RGB565);

		memset(eGFX_FrameBuffer,0, sizeof(eGFX_FrameBuffer));

	}


	/* Reset LCD and wait for reset to complete */
	Chip_RGU_TriggerReset(RGU_LCD_RST);

	while (Chip_RGU_InReset(RGU_LCD_RST))
	{
	}

    LCD_CONFIG_T LCD_Config;




    Chip_SCU_PinMux(LCD_DC_DEVICE_PIN, (SCU_PINIO_FAST),LCD_DC_DEVICE_PIN_MUX);
    Chip_GPIO_SetPinDIROutput(LPC_GPIO_PORT, LCD_DC_GPIO);

    Chip_SCU_PinMux(LCD_RESET_N_DEVICE_PIN, (SCU_PINIO_FAST),LCD_RESET_N_DEVICE_PIN_MUX);
    Chip_GPIO_SetPinDIROutput(LPC_GPIO_PORT, LCD_RESET_N_GPIO);

    Chip_SCU_PinMux(LCD_CS_DEVICE_PIN, (SCU_PINIO_FAST),LCD_CS_DEVICE_PIN_MUX);
    Chip_GPIO_SetPinDIROutput(LPC_GPIO_PORT, LCD_CS_GPIO);


    LCD_DC_INACTIVE;
    LCD_RESET_N_INACTIVE;
    LCD_CS_INACTIVE;


    //We assume the I/O pins have alread been setup
    Chip_Clock_Enable(CLK_MX_SSP1);
    Chip_SSP_Set_Mode(LPC_SSP1, SSP_MODE_MASTER);
    Chip_SSP_SetFormat(LPC_SSP1, SSP_BITS_8, SSP_FRAMEFORMAT_SPI, SSP_CLOCK_CPHA0_CPOL0);
    Chip_SSP_SetBitRate(LPC_SSP1, 15000000);
    Chip_SSP_Enable(LPC_SSP1);

	ST7701S_Initial();


    //This is for the BuyDisplayLCD
    LCD_Config.HBP = 16;	/*!< Horizontal back porch in clocks */
    LCD_Config.HFP = 20;	/*!< Horizontal front porch in clocks */
    LCD_Config.HSW = 12 ;	/*!< HSYNC pulse width in clocks */
    LCD_Config.PPL = 480;	/*!< Pixels per line */

    LCD_Config.VBP = 7;	/*!< Vertical back porch in clocks */
    LCD_Config.VFP = 30;	/*!< Vertical front porch in clocks */
    LCD_Config.VSW = 4;	/*!< VSYNC pulse width in clocks */
    LCD_Config.LPP= 480;	/*!< Lines per panel */
    LCD_Config.IOE = 0;	/*!< Invert output enable, 1 = invert */
    LCD_Config.IPC = 1;	/*!< Invert panel clock, 1 = invert */
    LCD_Config.IHS =1;	/*!< Invert HSYNC, 1 = invert */
    LCD_Config.IVS = 1;	/*!< Invert VSYNC, 1 = invert */
    LCD_Config.ACB = 1;	/*!< AC bias frequency in clocks (not used) */
    LCD_Config.BPP = 6;	/*!< Maximum bits per pixel the display supports */
    LCD_Config.LCD = LCD_TFT;;	/*!< LCD panel type */
    LCD_Config.color_format = LCD_COLOR_FORMAT_BGR;	/*!<BGR or RGB */
    LCD_Config.Clock = 9000000;
    LCD_Config.Dual = 0;	/*!< Dual panel, 1 = dual panel display */

   	Chip_LCD_Init(LPC_LCD, (LCD_CONFIG_T *) &LCD_Config);

   	Chip_LCD_EnableInts(LPC_LCD,LCD_INTMSK_LNBUIM);

   	Chip_LCD_SetUPFrameBuffer(LPC_LCD,  (void *) eGFX_BackBuffer[0].Data);

   	Chip_LCD_PowerOn(LPC_LCD);

   	NVIC_EnableIRQ(LCD_IRQn);

    Chip_GPDMA_Init(LPC_GPDMA);

    NVIC_EnableIRQ(DMA_IRQn);
}

volatile  uint32_t V_SyncFlag = 0;


volatile eGFX_ImagePlane *Q_Image = &eGFX_BackBuffer[0];

void LCD_IRQHandler()

{
    if (LPC_LCD->INTSTAT & LCD_INTMSK_LNBUIM)
    {
    	Chip_LCD_ClearInts(LPC_LCD, LCD_INTMSK_LNBUIM);
    	V_SyncFlag = 1;
	}

    __DSB();
}

void eGFX_WaitForV_Sync()
{
	V_SyncFlag = 0;

   while(V_SyncFlag == 0)
   {

   }
}

uint32_t eGFX_V_SyncReady()
{
	return V_SyncFlag;
}

void eGFX_Dump(eGFX_ImagePlane *Image)
{

	 LPC_LCD->UPBASE  = (uint32_t)Image->Data;
	 eGFX_WaitForV_Sync();

}

void eGFX_SetBacklight(uint8_t BacklightValue)
{



}

volatile uint32_t DMA_Complete;

void DMA_IRQHandler()
{

	uint32_t IntStatus = LPC_GPDMA->INTTCSTAT;

	LPC_GPDMA->INTTCCLEAR = IntStatus;
	DMA_Complete = true;

}

DMA_TransferDescriptor_t ImgDesc[480];


void eGFX_WEAK eGFX_Blit(eGFX_ImagePlane *Destination,
	int32_t x,
	int32_t y,
	const eGFX_ImagePlane *Sprite)
{


	int32_t X1_DstRegion = x;
	int32_t Y1_DstRegion = y;
	int32_t X2_DstRegion = x + Sprite->SizeX - 1;
	int32_t Y2_DstRegion = y + Sprite->SizeY - 1;

	//Reject offscreen

	if (X1_DstRegion >= Destination->SizeX)
		return;

	if (X2_DstRegion < 0)
		return;

	if (Y1_DstRegion >= Destination->SizeY)
		return;

	if (Y2_DstRegion < 0)
		return;


	int SrcStartX = 0;
	int SrcStartY = 0;

	int SrcStopX = Sprite->SizeX - 1;
	int SrcStopY = Sprite->SizeY - 1;


	if (X1_DstRegion < 0)
	{
		SrcStartX -= X1_DstRegion;
		X1_DstRegion = 0;
	}

	if (X2_DstRegion >= Destination->SizeX)
	{
		SrcStopX -= (X2_DstRegion - Destination->SizeX) - 1;
		X2_DstRegion = Destination->SizeX - 1;

	}

	if (Y1_DstRegion < 0)
	{
		SrcStartY -= Y1_DstRegion;
		Y1_DstRegion = 0;
	}

	if (Y2_DstRegion >= Destination->SizeY)
	{
		SrcStopY -= (Y2_DstRegion - Destination->SizeY) - 1;
		Y2_DstRegion = Destination->SizeY - 1;
	}

	uint16_t * SpriteData = (uint16_t *)Sprite->Data;
	uint16_t * DestinationData = (uint16_t *)Destination->Data;

	int NumLines = SrcStopY - SrcStartY;

	if(NumLines <= 0)
		return;

	// setup DMA for each line
	for(int i=0; i< Sprite->SizeY;i++)
	{
		ImgDesc[i].src =(uint32_t)(&SpriteData[((SrcStartY + i)*Sprite->SizeX) + SrcStartX]);
		ImgDesc[i].dst =(uint32_t)(&DestinationData[(Y1_DstRegion + i) * Destination->SizeX + X1_DstRegion]);

		ImgDesc[i].ctrl = (SrcStopX - SrcStartX) | (1<<18) | (1<<21) | (1<<26) | (1<<27);

		if(i == (NumLines - 1))
		{
			ImgDesc[i].ctrl|= 1<<31;
			ImgDesc[i].lli = 0;
		}
		else
		{
			ImgDesc[i].lli = (uint32_t)(&ImgDesc[i+1]);
		}

	}

	DMA_Complete = 0;

	Chip_GPDMA_SGTransfer(LPC_GPDMA,7,&ImgDesc[0], GPDMA_TRANSFERTYPE_M2M_CONTROLLER_DMA);

	while(DMA_Complete == 0)
	{

	}


}


/*
This is used to copy a section from equal sized image planes
when restoring a rectangle background image
*/
void  eGFX_BlitRestore(eGFX_ImagePlane *Destination,
								eGFX_Rect *Region,
							 eGFX_ImagePlane *Source)
{

	/*
		Clip the region
	*/
	if (Region->P1.X < 0)
		Region->P1.X = 0;

	if (Region->P1.Y < 0)
		Region->P1.Y = 0;

	if (Region->P2.X > (Destination->SizeX-1))
		Region->P2.X = Destination->SizeX-1;

	if (Region->P2.Y > (Destination->SizeY - 1))
		Region->P2.Y = (Destination->SizeY - 1);


	uint16_t * SrcData = (uint16_t *)Source->Data;
	uint16_t * DestinationData = (uint16_t *)Destination->Data;


	/*
	if(Region->P2.Y== Region->P1.Y)
	{
		return;
	}
	if(Region->P2.X == Region->P1.X)
	{
		return;
	}*/

		int NumLines = Region->P2.Y - Region->P1.Y + 1;
		int NumPixels = Region->P2.X - Region->P1.X + 1;

		if(NumLines<=0)
		{
			return;
		}

		if(NumPixels <= 0)
		{
			return;
		}

	// setup DMA for each line
	for(int i=0; i< NumLines ;i++)
	{
		ImgDesc[i].src =(uint32_t)(&SrcData[((Region->P1.Y + i)*Source->SizeX) + Region->P1.X]);
		ImgDesc[i].dst =(uint32_t)(&DestinationData[(Region->P1.Y + i) * Source->SizeX + Region->P1.X]);

		ImgDesc[i].ctrl = NumPixels | (1<<18) | (1<<21) | (1<<26) | (1<<27);

		if(i == (NumLines-1))
		{
			ImgDesc[i].lli = 0;
			ImgDesc[i].ctrl|= 1<<31;
		}
		else
		{
			ImgDesc[i].lli = (uint32_t)(&ImgDesc[i+1]);
		}

	}

	DMA_Complete = 0;

	Chip_GPDMA_SGTransfer(LPC_GPDMA,0,&ImgDesc[0], GPDMA_TRANSFERTYPE_M2M_CONTROLLER_DMA);

	while(DMA_Complete == 0)
	{

	}
}


#endif
