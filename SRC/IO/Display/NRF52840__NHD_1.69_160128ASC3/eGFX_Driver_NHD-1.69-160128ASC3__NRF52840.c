#include "../../eGFX.h"

#ifdef eGFX_DRIVER_169160128ASC2__NRF52840

#include "Board.h"
#include "System.h"

eGFX_ImagePlane eGFX_BackBuffer;

uint8_t BackBufferStoreA[eGFX_CALCULATE_16BPP_IMAGE_STORAGE_SPACE_SIZE(eGFX_PHYSICAL_SCREEN_SIZE_X,eGFX_PHYSICAL_SCREEN_SIZE_Y)];
uint8_t BackBufferStoreB[eGFX_CALCULATE_16BPP_IMAGE_STORAGE_SPACE_SIZE(eGFX_PHYSICAL_SCREEN_SIZE_X,eGFX_PHYSICAL_SCREEN_SIZE_Y)];

uint32_t NextLocation = 0;
volatile uint32_t GFX_Active = 0;
uint8_t Junk[128] ;
uint8_t * ImagePtr;

void OLED_SPI_TX_ONE_BYTE(uint8_t Data)
{
    Junk[0] = OLED_SPIM->RXD;
    Junk[0] = OLED_SPIM->RXD;
    OLED_SPIM->EVENTS_READY = 0;
    OLED_SPIM->TXD = Data;

    while(OLED_SPIM->EVENTS_READY == 0)
    {
    }

    OLED_SPIM->EVENTS_READY = 0;
    Junk[0] = OLED_SPIM->RXD;
}

void OLED_Command_160128RGB(unsigned char c)        // send command to OLED
{
    unsigned char i;
  
    unsigned char mask = 0x80;
    
    OLED_RS_LOW;
  
    OLED_CS_LOW;
    
    OLED_SPI_TX_ONE_BYTE(c);

    OLED_CS_HIGH;
}

void OLED_Data_160128RGB(unsigned char d)        // send data to OLED
{
    unsigned char i;
    unsigned char mask = 0x80;

     OLED_RS_HIGH;
     OLED_CS_LOW;
     OLED_SPI_TX_ONE_BYTE(d);
     OLED_CS_HIGH;
}

void OLED_SerialPixelData_160128RGB(unsigned char d)    // serial write for pixel data
{
    unsigned char i;
    unsigned char mask = 0x80;
  
    OLED_RS_HIGH;
    OLED_CS_LOW;
    OLED_SPI_TX_ONE_BYTE(d);
    OLED_CS_HIGH;

}

void OLED_SetColumnAddress_160128RGB(unsigned char x_start, unsigned char x_end)    // set column address start + end
{
    OLED_Command_160128RGB(0x17);
    OLED_Data_160128RGB(x_start);
    OLED_Command_160128RGB(0x18);
    OLED_Data_160128RGB(x_end);
}

void OLED_SetRowAddress_160128RGB(unsigned char y_start, unsigned char y_end)    // set row address start + end
{
    OLED_Command_160128RGB(0x19);
    OLED_Data_160128RGB(y_start);
    OLED_Command_160128RGB(0x1A);
    OLED_Data_160128RGB(y_end);
}

void OLED_WriteMemoryStart_160128RGB(void)    // write to RAM command
{
    OLED_Command_160128RGB(0x22);
}


void OLED_Pixel_160128RGB(unsigned long color)    // write one pixel of a given color
{

            OLED_SerialPixelData_160128RGB((color>>16));
            OLED_SerialPixelData_160128RGB((color>>8));
            OLED_SerialPixelData_160128RGB(color);
}

void OLED_SetPosition_160128RGB(unsigned char x_pos, unsigned char y_pos)    // set x,y address
{
    OLED_Command_160128RGB(0x20);
    OLED_Data_160128RGB(x_pos);
    OLED_Command_160128RGB(0x21);
    OLED_Data_160128RGB(y_pos);
}

void OLED_Init_160128RGB(void)      //OLED initialization
{
    OLED_RESET_LOW;
    Delay_mS(10);
    OLED_RESET_HIGH;
    Delay_mS(10);
    
    // display off, analog reset
    OLED_Command_160128RGB(0x04);
    OLED_Data_160128RGB(0x01);
    Delay_mS(1);
    
     // normal mode
    OLED_Command_160128RGB(0x04); 
    OLED_Data_160128RGB(0x00); 
    Delay_mS(10);
    
    // display off
    OLED_Command_160128RGB(0x06);
    OLED_Data_160128RGB(0x00);
    Delay_mS(10);
    
    // turn on internal oscillator using external resistor
    OLED_Command_160128RGB(0x02);
    OLED_Data_160128RGB(0x01); 
    
    // 90 hz frame rate, divider 0
    OLED_Command_160128RGB(0x03);
    OLED_Data_160128RGB(0x30); 
    
    // duty cycle 127
    OLED_Command_160128RGB(0x28);
    OLED_Data_160128RGB(0x7F);
    
    // start on line 0
    OLED_Command_160128RGB(0x29);
    OLED_Data_160128RGB(0x00); 
    
    // rgb_if
    OLED_Command_160128RGB(0x14);
    OLED_Data_160128RGB(0x31); 
    
    // Set Memory Write Mode
    OLED_Command_160128RGB(0x16);
    OLED_Data_160128RGB(0x66);
    
    // driving current r g b (uA)
    OLED_Command_160128RGB(0x10);
    OLED_Data_160128RGB(0x45);
    OLED_Command_160128RGB(0x11);
    OLED_Data_160128RGB(0x34);
    OLED_Command_160128RGB(0x12);
    OLED_Data_160128RGB(0x33);
    
    // precharge time r g b
    OLED_Command_160128RGB(0x08);
    OLED_Data_160128RGB(0x04);
    OLED_Command_160128RGB(0x09);
    OLED_Data_160128RGB(0x05);
    OLED_Command_160128RGB(0x0A);
    OLED_Data_160128RGB(0x05);
    
    // precharge current r g b (uA)
    OLED_Command_160128RGB(0x0B);
    OLED_Data_160128RGB(0x9D);
    OLED_Command_160128RGB(0x0C);
    OLED_Data_160128RGB(0x8C);
    OLED_Command_160128RGB(0x0D);
    OLED_Data_160128RGB(0x57);
    
    // Set Reference Voltage Controlled by External Resister
    OLED_Command_160128RGB(0x80);
    OLED_Data_160128RGB(0x00);
    
    // mode set
    OLED_Command_160128RGB(0x13);
    OLED_Data_160128RGB(0xA0);
    
    OLED_SetColumnAddress_160128RGB(0, 159);
    OLED_SetRowAddress_160128RGB(0, 127);

    // Display On
    OLED_Command_160128RGB(0x06);
    OLED_Data_160128RGB(0x01); 
}


#define OLED_RS_PORT        NRF_P1
#define OLED_RS_PORT_NUMBER 1
#define OLED_RS_PIN      8
#define OLED_RS_HIGH    OLED_RS_PORT->OUTSET = 1<<OLED_RS_PIN
#define OLED_RS_LOW     OLED_RS_PORT->OUTCLR = 1<<OLED_RS_PIN

#define OLED_CS_PORT         NRF_P1
#define OLED_CS_PORT_NUMBER  1
#define OLED_CS_PIN   11
#define OLED_CS_HIGH  OLED_CS_PORT->OUTSET = 1<<OLED_CS_PIN
#define OLED_CS_LOW    OLED_CS_PORT->OUTCLR = 1<<OLED_CS_PIN

#define OLED_RESET_PORT         NRF_P1
#define OLED_RESET_PORT_NUMBER  1
#define OLED_RESET_PIN   10
#define OLED_RESET_HIGH   OLED_RESET_PORT->OUTSET = 1<<OLED_RESET_PIN
#define OLED_RESET_LOW    OLED_RESET_PORT->OUTCLR = 1<<OLED_RESET_PIN

#define OLED_CLOCK_PORT  NRF_P1
#define OLED_CLOCK_PORT_NUMBER  1
#define OLED_CLOCK_PIN   7
#define OLED_CLOCK_HIGH  OLED_CLOCK_PORT->OUTSET = 1<<OLED_CLOCK_PIN
#define OLED_CLOCK_LOW    OLED_CLOCK_PORT->OUTCLR = 1<<OLED_CLOCK_PIN

#define OLED_DATA_PORT  NRF_P1
#define OLED_DATA_PORT_NUMBER 1
#define OLED_DATA_PIN   6
#define OLED_DATA_HIGH  OLED_DATA_PORT->OUTSET = 1<<OLED_DATA_PIN
#define OLED_DATA_LOW   OLED_DATA_PORT->OUTCLR = 1<<OLED_DATA_PIN

#define OLED_SPIM   NRF_SPI2

#define OUTPUT_PIN_CONFIG ((GPIO_PIN_CNF_SENSE_Disabled << GPIO_PIN_CNF_SENSE_Pos)   | \
                        (GPIO_PIN_CNF_DRIVE_S0S1 << GPIO_PIN_CNF_DRIVE_Pos)       | \
                        (GPIO_PIN_CNF_PULL_Disabled << GPIO_PIN_CNF_PULL_Pos)     | \
                        (GPIO_PIN_CNF_INPUT_Disconnect << GPIO_PIN_CNF_INPUT_Pos) | \
                        (GPIO_PIN_CNF_DIR_Output << GPIO_PIN_CNF_DIR_Pos))

#define SPIM_SCK_OUTPUT_PIN_CONFIG ((GPIO_PIN_CNF_SENSE_Disabled << GPIO_PIN_CNF_SENSE_Pos)   | \
                        (GPIO_PIN_CNF_DRIVE_S0S1 << GPIO_PIN_CNF_DRIVE_Pos)       | \
                        (GPIO_PIN_CNF_PULL_Disabled << GPIO_PIN_CNF_PULL_Pos)     | \
                        (GPIO_PIN_CNF_INPUT_Connect << GPIO_PIN_CNF_INPUT_Pos) | \
                        (GPIO_PIN_CNF_DIR_Output << GPIO_PIN_CNF_DIR_Pos))

void eGFX_InitDriver()
{


    OLED_RS_PORT->PIN_CNF[OLED_RS_PIN] = OUTPUT_PIN_CONFIG;
    OLED_RESET_PORT->PIN_CNF[OLED_RESET_PIN] = OUTPUT_PIN_CONFIG;

    OLED_CS_PORT->PIN_CNF[OLED_CS_PIN] = OUTPUT_PIN_CONFIG;
    OLED_CLOCK_PORT->PIN_CNF[OLED_CLOCK_PIN] = SPIM_SCK_OUTPUT_PIN_CONFIG;
    OLED_DATA_PORT->PIN_CNF[OLED_DATA_PIN] = OUTPUT_PIN_CONFIG;

    OLED_SPIM->ENABLE = 0;
    OLED_SPIM->PSEL.SCK = NRF_GPIO_PIN_MAP(OLED_CLOCK_PORT_NUMBER,OLED_CLOCK_PIN);
    OLED_SPIM->PSEL.MOSI = NRF_GPIO_PIN_MAP(OLED_DATA_PORT_NUMBER,OLED_DATA_PIN);
   // OLED_SPIM->PSEL.CSN= NRF_GPIO_PIN_MAP(OLED_CS_PORT_NUMBER,OLED_CS_PIN);
    OLED_SPIM->FREQUENCY = SPI_FREQUENCY_FREQUENCY_M8;

    OLED_SPIM->CONFIG = 0;
   // OLED_SPIM->CSNPOL = 0;

    OLED_SPIM->ENABLE = 1;

    OLED_RESET_HIGH;
    OLED_RS_HIGH;
    OLED_CS_HIGH;
    OLED_DATA_LOW;
    OLED_CLOCK_LOW;


     OLED_Init_160128RGB();
     eGFX_ImagePlaneInit(&eGFX_BackBuffer, BackBufferStoreA, eGFX_PHYSICAL_SCREEN_SIZE_X,eGFX_PHYSICAL_SCREEN_SIZE_Y, eGFX_IMAGE_PLANE_16BPP_3553_GRBG);
     GFX_Active = 0;
}


void SPIM2_SPIS2_SPI2_IRQHandler()
{

  Junk[0] = OLED_SPIM->RXD;

  OLED_SPIM->EVENTS_READY = 0;

  NextLocation++;

  if(NextLocation < 40960)
    {
      OLED_SPIM->TXD = ImagePtr[NextLocation];
    }
    else
    {
   
        OLED_SPIM->INTENCLR = 0xFF;
        NVIC_DisableIRQ(SPIM2_SPIS2_SPI2_IRQn);
        OLED_CS_HIGH;
        GFX_Active = 0;
    }

}

void eGFX_WaitForV_Sync()
{
    
}

/*
    Misc Notes:

    8m is the magic baud rate as the display *needs* some idle time between bytes.
      back to back bytes makes the display do off things. We are purposely not using
      the double buffered spi to ensure there is some idle time when the CPU jumps to 
      the SPI interrupt.

*/

void eGFX_Dump(eGFX_ImagePlane *Image)
{
    //Wait for the last operation to complete
    while( GFX_Active == 1)
    {
    }

    OLED_SetPosition_160128RGB(0,0);
    
    OLED_WriteMemoryStart_160128RGB();
  
    OLED_RS_HIGH;
    
    OLED_CS_LOW;
 
    ImagePtr = &Image->Data[0];
  
    if(Image->Data == BackBufferStoreA)
    {
         Image->Data = BackBufferStoreB;
    }
    else
    {
        Image->Data = BackBufferStoreA;
    }

    GFX_Active = 1;

    Junk[0] = OLED_SPIM->RXD;
    
    Junk[0] = OLED_SPIM->RXD;

    OLED_SPIM->EVENTS_READY = 0;
   
    NextLocation = 1;
    
    OLED_SPIM->TXD = ImagePtr[0];
    
    OLED_SPIM->TXD = ImagePtr[1];

    OLED_SPIM->INTENSET = 0xFF;//SPI_EVENTS_READY_EVENTS_READY_Msk;

    NVIC_EnableIRQ(SPIM2_SPIS2_SPI2_IRQn);
  
}


void eGFX_SetBacklight(uint8_t BacklightValue)
{



}

#endif
