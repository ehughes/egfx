#include "../../eGFX.h"

#ifdef eGFX_DRIVER_27_MEM_LCD__LPC43xx

#ifdef  CORE_M4
	#include "System.h"
#endif
#include <cr_section_macros.h>
#include "Chip.h"
#include "System/Drivers/IPC.h"



//************************************************************************
//Functions/Macros for porting to other CPUS...
//************************************************************************

#define DB0_GPIO	7,0
#define DB0_PIN		0xE,0
#define DB0_MUX		FUNC4

#define DB1_GPIO	7,1
#define DB1_PIN		0xE,1
#define DB1_MUX		FUNC4

#define DB2_GPIO	7,2
#define DB2_PIN		0xE,2
#define DB2_MUX		FUNC4

#define DB3_GPIO	7,3
#define DB3_PIN		0xE,3
#define DB3_MUX		FUNC4

#define DB4_GPIO	7,4
#define DB4_PIN		0xE,4
#define DB4_MUX		FUNC4

#define DB5_GPIO	7,5
#define DB5_PIN		0xE,5
#define DB5_MUX		FUNC4

#define DB6_GPIO	7,6
#define DB6_PIN		0xE,6
#define DB6_MUX		FUNC4

#define DB7_GPIO	7,7
#define DB7_PIN		0xE,7
#define DB7_MUX		FUNC4

#define RW_GPIO		7,9
#define RW_PIN		0xE,9
#define RW_MUX		FUNC4

#define DC_GPIO		7,10
#define DC_PIN		0xE,10
#define DC_MUX		FUNC4

#define RES_GPIO	7,11
#define RES_PIN		0xE,11
#define RES_MUX		FUNC4

#define E_GPIO		7,8
#define E_PIN		0xE,8
#define E_MUX		FUNC4

#define CS_GPIO		7,13
#define CS_PIN		0xE,13
#define CS_MUX		FUNC4


#define MEMLCD_CS_DEVICE_PIN					0xE,13
#define	MEMLCD_CS_DEVICE_PIN_MUX				FUNC4
#define MEMLCD_CS_GPIO_PORT					    7
#define MEMLCD_CS_GPIO_PIN						13

#define MEMLCD_CS_GPIO		MEMLCD_CS_GPIO_PORT,MEMLCD_CS_GPIO_PIN

#define MEMLCD_CS_INACTIVE			Chip_GPIO_WritePortBit(LPC_GPIO_PORT,MEMLCD_CS_GPIO_PORT,MEMLCD_CS_GPIO_PIN,0);
#define MEMLCD_CS_ACTIVE			Chip_GPIO_WritePortBit(LPC_GPIO_PORT,MEMLCD_CS_GPIO_PORT,MEMLCD_CS_GPIO_PIN,1);


#define NHD_312_25664_DELAY_MS(x)	//Delay_mS(x)
#define NHD_312_25664_DELAY_US(x)	//Delay_uS(x)

#define NHD_312_25664_SET_DC		Chip_GPIO_WritePortBit(LPC_GPIO_PORT,DC_GPIO, 1)
#define NHD_312_25664_CLEAR_DC		Chip_GPIO_WritePortBit(LPC_GPIO_PORT,DC_GPIO, 0)

#define NHD_312_25664_SET_RES		Chip_GPIO_WritePortBit(LPC_GPIO_PORT,RES_GPIO, 1)
#define NHD_312_25664_CLEAR_RES		Chip_GPIO_WritePortBit(LPC_GPIO_PORT,RES_GPIO, 0)

#define NHD_312_25664_SET_RW		Chip_GPIO_WritePortBit(LPC_GPIO_PORT,RW_GPIO, 1)
#define NHD_312_25664_CLEAR_RW		Chip_GPIO_WritePortBit(LPC_GPIO_PORT,RW_GPIO, 0)

#define NHD_312_25664_SET_E			Chip_GPIO_WritePortBit(LPC_GPIO_PORT,E_GPIO, 1)
#define NHD_312_25664_CLEAR_E		Chip_GPIO_WritePortBit(LPC_GPIO_PORT,E_GPIO, 0)

#define NHD_312_25664_SET_CS			Chip_GPIO_WritePortBit(LPC_GPIO_PORT,CS_GPIO, 1)
#define NHD_312_25664_CLEAR_CS			Chip_GPIO_WritePortBit(LPC_GPIO_PORT,CS_GPIO, 0)


#define MEMLCD_5V_ENABLE			NHD_312_25664_SET_E
#define MEMLCD_5V_DISABLE			NHD_312_25664_CLEAR_E

static const unsigned char bitReverse[] =
{
    0x00, 0x80, 0x40, 0xC0, 0x20, 0xA0, 0x60, 0xE0, 0x10, 0x90, 0x50, 0xD0, 0x30, 0xB0, 0x70, 0xF0,
    0x08, 0x88, 0x48, 0xC8, 0x28, 0xA8, 0x68, 0xE8, 0x18, 0x98, 0x58, 0xD8, 0x38, 0xB8, 0x78, 0xF8,
    0x04, 0x84, 0x44, 0xC4, 0x24, 0xA4, 0x64, 0xE4, 0x14, 0x94, 0x54, 0xD4, 0x34, 0xB4, 0x74, 0xF4,
    0x0C, 0x8C, 0x4C, 0xCC, 0x2C, 0xAC, 0x6C, 0xEC, 0x1C, 0x9C, 0x5C, 0xDC, 0x3C, 0xBC, 0x7C, 0xFC,
    0x02, 0x82, 0x42, 0xC2, 0x22, 0xA2, 0x62, 0xE2, 0x12, 0x92, 0x52, 0xD2, 0x32, 0xB2, 0x72, 0xF2,
    0x0A, 0x8A, 0x4A, 0xCA, 0x2A, 0xAA, 0x6A, 0xEA, 0x1A, 0x9A, 0x5A, 0xDA, 0x3A, 0xBA, 0x7A, 0xFA,
    0x06, 0x86, 0x46, 0xC6, 0x26, 0xA6, 0x66, 0xE6, 0x16, 0x96, 0x56, 0xD6, 0x36, 0xB6, 0x76, 0xF6,
    0x0E, 0x8E, 0x4E, 0xCE, 0x2E, 0xAE, 0x6E, 0xEE, 0x1E, 0x9E, 0x5E, 0xDE, 0x3E, 0xBE, 0x7E, 0xFE,
    0x01, 0x81, 0x41, 0xC1, 0x21, 0xA1, 0x61, 0xE1, 0x11, 0x91, 0x51, 0xD1, 0x31, 0xB1, 0x71, 0xF1,
    0x09, 0x89, 0x49, 0xC9, 0x29, 0xA9, 0x69, 0xE9, 0x19, 0x99, 0x59, 0xD9, 0x39, 0xB9, 0x79, 0xF9,
    0x05, 0x85, 0x45, 0xC5, 0x25, 0xA5, 0x65, 0xE5, 0x15, 0x95, 0x55, 0xD5, 0x35, 0xB5, 0x75, 0xF5,
    0x0D, 0x8D, 0x4D, 0xCD, 0x2D, 0xAD, 0x6D, 0xED, 0x1D, 0x9D, 0x5D, 0xDD, 0x3D, 0xBD, 0x7D, 0xFD,
    0x03, 0x83, 0x43, 0xC3, 0x23, 0xA3, 0x63, 0xE3, 0x13, 0x93, 0x53, 0xD3, 0x33, 0xB3, 0x73, 0xF3,
    0x0B, 0x8B, 0x4B, 0xCB, 0x2B, 0xAB, 0x6B, 0xEB, 0x1B, 0x9B, 0x5B, 0xDB, 0x3B, 0xBB, 0x7B, 0xFB,
    0x07, 0x87, 0x47, 0xC7, 0x27, 0xA7, 0x67, 0xE7, 0x17, 0x97, 0x57, 0xD7, 0x37, 0xB7, 0x77, 0xF7,
    0x0F, 0x8F, 0x4F, 0xCF, 0x2F, 0xAF, 0x6F, 0xEF, 0x1F, 0x9F, 0x5F, 0xDF, 0x3F, 0xBF, 0x7F, 0xFF
};
/*
static uint8_t bitReverse(uint8_t bData)
{
  uint8_t lookup[16] = { 0, 8, 4, 12, 2, 10, 6, 14, 1, 9, 5, 13, 3, 11, 7, 15 };
  return (uint8_t)(((lookup[(bData & 0x0F)]) << 4) + lookup[((bData & 0xF0) >> 4)]);
}
*/



uint8_t Junk2;


uint8_t FrameInversionFlag = 0x00;

static inline uint8_t MEMLCD_SPI_RW(uint8_t Data)
{
	uint32_t Retries = 0;

    //LPC_SPI->DR = bitReverse[Data];
    LPC_SPI->DR = Data;
    while((LPC_SPI->SR & SPI_SR_SPIF) == 0)
    {
    	Retries++;
      	if(Retries>100000)
    		break;
    }

    return LPC_SPI->DR;
}


void MemoryLCD_DataMaintain()
{
    FrameInversionFlag ^= 0x02; // Toggle bit 2 to keep display happy
    MEMLCD_CS_ACTIVE;
    Junk2 =  MEMLCD_SPI_RW(FrameInversionFlag);
    Junk2 =  MEMLCD_SPI_RW(0);
    MEMLCD_CS_INACTIVE;
}

void MemoryLCD_Clear()
{
    FrameInversionFlag ^= 0x02; // Toggle bit 2 to keep display happy
    MEMLCD_CS_ACTIVE;
    Junk2 =  MEMLCD_SPI_RW(FrameInversionFlag | (1<<2));
    Junk2 = MEMLCD_SPI_RW(0);
    MEMLCD_CS_INACTIVE;
}




void eGFX_Dump(eGFX_ImagePlane *Image)
{

    uint16_t x,y;
    FrameInversionFlag ^= 0x02; // Toggle bit 2 to keep display happy

    FrameInversionFlag |= 0x01;


    for(y=0; y<eGFX_PHYSICAL_SCREEN_SIZE_Y; y++)
        {
    		MEMLCD_CS_ACTIVE;

    		Junk2 = MEMLCD_SPI_RW(FrameInversionFlag | 0x01);
            Junk = MEMLCD_SPI_RW(bitReverse[y+1]);

            for(x=0; x<150; x++)
                {
                   Junk2 = MEMLCD_SPI_RW(Image->Data[y*150 + x]);

                }

               Junk2 = MEMLCD_SPI_RW(0);
               Junk2 = MEMLCD_SPI_RW(0);

               MEMLCD_CS_INACTIVE;


        }


}

/*
void eGFX_Dump(eGFX_ImagePlane *Image)
{

    uint16_t x,y;
    FrameInversionFlag ^= 0x02; // Toggle bit 2 to keep display happy

    FrameInversionFlag |= 0x01;

    MEMLCD_CS_ACTIVE;
    Junk2 = MEMLCD_SPI_RW(FrameInversionFlag | 0x01);

    for(y=0; y<eGFX_PHYSICAL_SCREEN_SIZE_Y; y++)
        {
            Junk = MEMLCD_SPI_RW(y);

            for(x=0; x<50; x++)
                {
                   Junk2 = MEMLCD_SPI_RW(~(eGFX_ImagePlane_GetByte(Image, x*8 ,y)));

                   // Junk = MEMLCD_SPI_RW(0xF7);
                }

            //1 dummy writes
            Junk2 = MEMLCD_SPI_RW(0);
        }

    //1 dummy writes
    Junk2 = MEMLCD_SPI_RW(0);
    Junk2 = MEMLCD_SPI_RW(0);
    MEMLCD_CS_INACTIVE;
}
*/

#ifdef CORE_M4


eGFX_ImagePlane eGFX_BackBuffer;


//__BSS(M4_1) uint8_t BackBufferStore[eGFX_CALCULATE_1BPP_IMAGE_STORAGE_SPACE_SIZE(eGFX_PHYSICAL_SCREEN_SIZE_X, eGFX_PHYSICAL_SCREEN_SIZE_Y)];

__BSS(M4_1) uint8_t BackBufferStore[eGFX_CALCULATE_3BPP_IMAGE_STORAGE_SPACE_SIZE(eGFX_PHYSICAL_SCREEN_SIZE_X, eGFX_PHYSICAL_SCREEN_SIZE_Y)];

void eGFX_InitDriver()
{
    eGFX_ImagePlaneInit(&eGFX_BackBuffer,&BackBufferStore[0],eGFX_PHYSICAL_SCREEN_SIZE_X, eGFX_PHYSICAL_SCREEN_SIZE_Y,eGFX_IMAGE_PLANE_3BPP_RGB);


    Chip_Clock_Enable(CLK_SPI);

    Chip_SPI_SetMode(LPC_SPI, SPI_MODE_MASTER);

    LPC_SPI->CR = (LPC_SPI->CR & (~0xF1C)) | SPI_CR_BIT_EN | SPI_BITS_8 | SPI_CLOCK_CPHA0_CPOL0 | SPI_DATA_LSB_FIRST;

    Chip_SPI_SetBitRate(LPC_SPI, 8000000);

    //Clear out the SPI status

    Junk = LPC_SPI->SR;
    Junk = LPC_SPI->DR;


	Chip_SCU_PinMux(DB0_PIN,SCU_PINIO_FAST , DB0_MUX);
	Chip_GPIO_WriteDirBit(LPC_GPIO_PORT,DB0_GPIO, true);
    Chip_GPIO_WritePortBit(LPC_GPIO_PORT,DB0_GPIO, 1);

	Chip_SCU_PinMux(RES_PIN,SCU_PINIO_FAST , RES_MUX);
	Chip_GPIO_WriteDirBit(LPC_GPIO_PORT,RES_GPIO, true);
    Chip_GPIO_WritePortBit(LPC_GPIO_PORT,RES_GPIO, 1);

	Chip_SCU_PinMux(DC_PIN,SCU_PINIO_FAST , DC_MUX);
	Chip_GPIO_WriteDirBit(LPC_GPIO_PORT,DC_GPIO, true);
    Chip_GPIO_WritePortBit(LPC_GPIO_PORT,DC_GPIO, 0);

	 Chip_SCU_PinMux(E_PIN,SCU_PINIO_FAST , E_MUX);
	 Chip_GPIO_WriteDirBit(LPC_GPIO_PORT,E_GPIO, true);
	 MEMLCD_5V_DISABLE;


	 Chip_SCU_PinMux(MEMLCD_CS_DEVICE_PIN,SCU_PINIO_FAST , MEMLCD_CS_DEVICE_PIN_MUX);
   	 Chip_GPIO_WriteDirBit(LPC_GPIO_PORT,MEMLCD_CS_GPIO, true);
   	 MEMLCD_CS_INACTIVE;



}

#endif



void eGFX_SetBacklight(uint8_t BacklightValue)
{



}

void eGFX_WaitForV_Sync()
{

}



//This will flag the M0 to dump the image buffer
//void eGFX_Dump(eGFX_ImagePlane *Image)
//{

//	MyIPC.eGFX_BackBuffer = Image;
//	MyIPC.Msg_M4_M0 = IPC_MSG_DUMP_GFX;
 //   IPC_Poke();
//}



#endif
