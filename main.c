/*
 * main.c
 *
 *  Created on: 24-Oct-2022
 *      Author: naga.narasimha
 */

//*****************************************************************************

#include "ti/devices/msp432e4/inc/msp432e401y.h"
//C:\ti\simplelink_msp432e4_sdk_4_20_00_12\source\ti\devices\msp432e4\inc
//#include "ti/devices/msp432e4/driverlib/inc/hw_types.h"
#include "ti/devices/msp432e4/driverlib/inc/hw_gpio.h"

//#include "ti/devices/msp432e4/driverlib/inc/hw_memmap.h"



#include <Project_Source/ATP_SW/Include/mistIncludes.h>


//*****************************************************************************
//
// Defines for setting up the system clock.
//
//*****************************************************************************
#define SYSTICKHZ               100
#define SYSTICKMS               (1000 / SYSTICKHZ)

// MCU operating clock frequency
#define MCU_CLOCK_FREQ_120MHz         120000000u //120MHz

#define ETH_DATA_BUFF_SIZE        1050



#define FLASH_BUFF_LEN 256

#define FLASH_ADDR 0x00000000

#define MCU_PROCESS 1

#define FLASH_STRATING_ADD 0x00010000
//*****************************************************************************
//
// Interrupt priority definitions.  The top 3 bits of these values are
// significant with lower values indicating higher priority interrupts.
//
//*****************************************************************************
#define SYSTICK_INT_PRIORITY    0x80
#define ETHERNET_INT_PRIORITY   0xC0


//*****************************************************************************
//
// The system clock frequency.
//
//*****************************************************************************
UINT32 g_ui32SysClock;

//*****************************************************************************
//
// Volatile global flag to manage LED blinking, since it is used in interrupt
// and main application.  The LED blinks at the rate of SYSTICKHZ.
//
//*****************************************************************************
volatile bool g_bLED;

//*****************************************************************************
//
// The error routine that is called if the driver library encounters an error.
//
//*****************************************************************************
#ifdef DEBUG
void
__error__(char *pcFilename, uint32_t ui32Line)
{
}
#endif

//*****************************************************************************
//
// The interrupt handler for the SysTick interrupt.
//
//*****************************************************************************
void SysCtl_Handler(void)
{

    UINT32 u32IntStatus = 0;

    u32IntStatus = ROM_SysCtlIntStatus(true);

    ROM_SysCtlIntClear(SYSCTL_INT_MOSC_FAIL|SYSCTL_INT_PLL_LOCK);

    if((u32IntStatus & SYSCTL_INT_MOSC_FAIL) == SYSCTL_INT_MOSC_FAIL)
    {

        g_ui32Flags = 1;
        // ROM_SSIDataPut(SSI0_BASE,0XAA);

        //   printf("SYSCTL_INT_MOSC_FAIL IntStatus:%X\n",u32IntStatus);
    }

    if((u32IntStatus & SYSCTL_INT_PLL_LOCK) == SYSCTL_INT_PLL_LOCK)
    {

        // ROM_SSIDataPut(SSI0_BASE,0XBB);

        g_ui32Flag_VC = 1;
        //g_ui32Flags = u32IntStatus;
        // printf("SYSCTL_INT_PLL_LOCK IntStatus:%X\n",u32IntStatus);
    }

}

//*****************************************************************************
//
// The interrupt handler for the SysTick interrupt.
//
//*****************************************************************************
void SysTick_Handler(void)
{
    //
    // Call the lwIP timer handler.
    //
    lwIPTimer(SYSTICKMS);

}

//Global Variables

UINT8 gu8EthDataBuff[ETH_DATA_BUFF_SIZE];

//SINT16 RFIC_Data[6000];
//
//UINT8 u16index_1;


volatile UINT32 DataLength = 0;

UINT32 g_ui32Flags;
UINT32 g_ui32Flag_VC;
UINT32 g_ui32TimerCount;

#define MCU_MOSE_CLOCK

#define MCU_EXTERNAL_CLOCK

#define MCU_INT_CLOCK

#define MCU_ETHERNET

#define MCU_RFIC

#define MCU_FPGA_GPIO

#define MCU_EXT_GPIO

#define MCU_GPIO_LED

#define MCU_SPI_FLASH

#define MCU_SDRAM

#define IMU_SENSOR

#define GPIO_UNLOCK

#define PIN_PD7_AS_GPIO

#define PIN_PD7_AS_PER

//extern FLASH_DEVICE_OBJECT *fdo;
//
//extern FLASH_DEVICE_OBJECT Obj;



BNO055_IMU_DEVICE_OBJECT rObj;

UINT32 gu32XmodemReceiveLength;

#define BL_UART_MAX_IMAGE_SIZE    (1024*1024*1024)
//*****************************************************************************
//
// This example demonstrates the use of the Ethernet Controller.
//
//*****************************************************************************

int main(void)
{

    UINT32 u32I2CBase = 0;
    g_ui32Flags = 0;
    g_ui32Flag_VC = 0;
    UINT32 u32Baudrate = 0;

    UINT32 u32Mode = 0;

    UINT32 u32BitRate = 0;

    UINT8 u8SlaveAddr = 0;

    UINT32 u32milliseconds = 0;

    UINT32 u32RdFlashId = 0;

    UINT8 u8RetVal = 0;

    UINT32 data = 0;

    //Set Default FPGA Boot flag status
    gStrFPGABootCmdPkt.u8CmdFlagStatus = STOP;
    gStrSPIFlashCmdPkt.u8CmdFlagStatus = CMD_FLAG_STOP;
    gStrSDRAMCmdPkt.u8CmdFlagStatus = SDRAM_CMD_FLAG_STOP;
    gStrRFIC_TX_CmdPkt.u8CmdFlagStatus = RFIC_CMD_FLAG_STOP;
    gStrRFIC_RX_CmdPkt.u8CmdFlagStatus = RFIC_CMD_FLAG_STOP;
    gStrBootFlashCmdPkt.u8CmdFlagStatus = CMD_FLAG_STOP;


    /*****RFIC RX Data  Flag Disable *****/
    RFICDataReadFlag =0;

    /************************MCU clock Initialization ************************************/
    //
    // Make sure the main oscillator is enabled because this is required by
    // the PHY.  The system must have a 25MHz crystal attached to the OSC
    // pins. The SYSCTL_MOSC_HIGHFREQ parameter is used when the crystal
    // frequency is 10MHz or higher.
    //

    /******** MOSC CLK Configuration*******/

#ifdef MCU_MOSE_CLOCK_

    SysCtlIntRegister(SysCtl_Handler);

    ROM_SysCtlMOSCConfigSet(SYSCTL_MOSC_HIGHFREQ|SYSCTL_MOSC_INTERRUPT|SYSCTL_MOSC_VALIDATE);

    ROM_SysCtlIntEnable(SYSCTL_INT_MOSC_FAIL|SYSCTL_INT_PLL_LOCK);
#endif

#ifdef MCU_EXTERNAL_CLOCK

    ROM_SysCtlMOSCConfigSet(SYSCTL_MOSC_HIGHFREQ|SYSCTL_MOSC_SESRC);

    //
    // Run from the PLL at 120 MHz.
    //
    g_ui32SysClock = MAP_SysCtlClockFreqSet((SYSCTL_XTAL_25MHZ |
            SYSCTL_OSC_MAIN |
            SYSCTL_USE_PLL |
            SYSCTL_CFG_VCO_480),MCU_CLOCK_FREQ_120MHz);

#endif


#ifdef MCU_INT_CLOCK_
    /**  MCU SYS_CLK INT_PISOC, USE_OSC 16MHZ *********/
    g_ui32SysClock = MAP_SysCtlClockFreqSet((SYSCTL_OSC_INT |
            SYSCTL_USE_OSC |
            SYSCTL_CFG_VCO_480),16000000);

    //  u8RetVal = SysCtlVCOGet(uint32_t ui32Crystal, uint32_t *pui32VCOFrequency);

#endif


#ifdef UART_0
    /************************MCU UART-0 for console I/O Initialization **************************************/

    u32Baudrate = UART_BAUDRATE_115200;

    u8RetVal = UART0_Init(UART_BAUDRATE_115200);


    g_ui32Base = UART0_BASE; //Enable for console print


    /***** Select the required UART for console**********/


#endif

    printf("Hello BBHW3\n");


#ifdef UART_2_ // NAVIC IC
    /************************MCU UART-0 for console I/O Initialization **************************************/

    u32Baudrate = UART_BAUDRATE_115200;

    u8RetVal = UART2_Init(UART_BAUDRATE_115200);




#endif
    /****************NOTE : unlock the GPIO-D Port first before using PD7 pin any where**********/
#ifdef GPIO_UNLOCK

    /*****************Unlock the GPIO PORTD PD7 pin**************************************/
    /* Enable the clock to the GPIO Port A and wait for it to be ready */
    ROM_SysCtlPeripheralEnable(SYSCTL_PERIPH_GPIOD);
    while (!(ROM_SysCtlPeripheralReady(SYSCTL_PERIPH_GPIOD)))
    {
    }

    GPIOD->LOCK = GPIO_LOCK_KEY;

    GPIOD->CR |= 0x80;

#ifdef PIN_PD7_AS_GPIO_
    /******SET to use PD7 as GPIO************/
    GPIOD->AFSEL &= 0xf7;
#endif

#ifdef PIN_PD7_AS_PER
    /******SET to use PD7 not as GPIO************/
    GPIOD->AFSEL|= 0x80;
#endif

    GPIOD->LOCK = GPIO_LOCK_KEY;

#endif

#ifdef MCU_ETHERNET
    /************************MCU Ethernet Initialization **************************************/


    u8RetVal = Ethernet_Init();


#endif

#if 1

    UARTprintf("MCU_CLK_FREQ : %d \n",g_ui32SysClock);

#ifdef UART_1
    /************************MCU UART Module Initialization *****************************************/
    //
    //    // UART interface between FPGA - MCU
    u32Baudrate = UART_BAUDRATE_115200;

    u8RetVal = UART1_Init(UART_BAUDRATE_115200);
#endif

    /************************MCU SPI Module Initialization ************************************/

#ifdef SPI_0

    /************************MCU SPI MCU SPI-0 RFIC TX (MCU-FPGA) Module Initialization ************************************/

    // u32BitRate = (UINT32)((g_ui32SysClock/SPI0_BITRATE_DIV) * SPI0_BITRATE_MUL);

    u32BitRate = 20000000;

    UARTprintf("SPI-0:u32BitRate:%d \n",u32BitRate);

    u32Mode = SSI_MODE_MASTER;

    u8RetVal = SPI0_Init(u32BitRate,SSI_MODE_MASTER,SSI_FRF_MOTO_MODE_1,8);
#endif

#ifdef SPI_1

    /*************  MCU SPI-1 (MCU-FPGA) RFIC configuration and FPGA Boot in (slave serial mode)****************/

    //  AD9364 RFCI SPI MAX 50MHZ

    // u32BitRate = (UINT32)((g_ui32SysClock/SPI1_BITRATE_DIV) * SPI1_BITRATE_MUL);

    u32BitRate = 8000000;

    UARTprintf("SPI-1:u32BitRate:%d \n",u32BitRate);

    u32Mode = SSI_MODE_MASTER;

    u8RetVal = SPI1_Init(u32BitRate,SSI_MODE_MASTER,SSI_FRF_MOTO_MODE_0);

#endif

#ifdef SPI_2

    /*********** MCU SPI-3 RFIC RX (MCU-FPGA)*********************/

    //  u32BitRate = (UINT32)((g_ui32SysClock/SPI2_BITRATE_DIV) * SPI2_BITRATE_MUL);

    u32BitRate = 60000000;

    UARTprintf("SPI-2:u32BitRate:%d \n",u32BitRate);

    // u32Mode = SSI_MODE_SLAVE;

    u32Mode = SSI_MODE_MASTER;


    u8RetVal =  SPI2_Init(u32BitRate,u32Mode);

#endif


#ifdef MCU_I2C_2
    /************************MCU I2C  Module Initialization ******************************/

    /*********** IMU Sensor Module Initialization************/

    /*Master Mode configuration */
    u8RetVal = I2C2_Init();

#endif


#ifdef MCU_SPI_FLASH_


    /************************MCU External SPI NOR Flash - Module Initialization *****************************/


    u32BitRate = (UINT32)((g_ui32SysClock/SPI3_BITRATE_DIV) * SPI3_BITRATE_MUL);

    u32BitRate = 60000000;
    uint32 Buff[20];


    CharStream char_stream_send;
    CharStream char_stream_recv;
    uint8  cRDID;// = SPI_FLASH_INS_PP;

    uint8 cPER = 0x35;
    //ReturnType ret;
    uint8 flag;

    uint8  pIdentification[FLASH_BUFF_LEN+10];
    uint8_t RecvBuff[FLASH_BUFF_LEN + 10];
    uint32 z;

    // delayms(20000);

    ParameterType ProgmPointer;



    UARTprintf("SPI-3:u32BitRate:%d \n",u32BitRate);

    //GPIO Pins set as output
    u8RetVal = GPIO_Output_Init(SYSCTL_PERIPH_GPIOP,GPIO_PORTP_BASE,(GPIO_PIN_1 | GPIO_PIN_0),(GPIO_PIN_1 | GPIO_PIN_0));//For BBHW1

    // u8RetVal = GPIO_Output_Init(SYSCTL_PERIPH_GPIOF,GPIO_PORTF_BASE,(GPIO_PIN_3|GPIO_PIN_4),(GPIO_PIN_3|GPIO_PIN_4));//For BBHW3



    ReturnType ret;


#if 1

    u8RetVal = SPI_NOR_Flash_Init(&Obj,u32BitRate,&u32RdFlashId);
    //-----------------------Enabling QUAD MODE-------------------------//
    UARTprintf("Enabling Quad Mode....\n\r");


    ret = FlashWriteVEConfigReg(0x7F);
    delayms(200000);
    UARTprintf("Enabling Quad Mode with ret: %d\n\r",ret);
    gu8QuadModeEnable = 1;

#endif




    ret = SpiInit( u32BitRate); /* initialize your SPI interface */

    ret = Driver_Init(&Obj); /* initialize the flash driver */





#if 1
    //--------------------------------------------MISTRAL--------------------------------------------//

    //----------------CODE TO ENABLE 4- BYTE BODE---------------------------------------//


    //CharStream char_stream_send;
    cPER = SPI_FLASH_INS_EN4BYTEADDR;
    //ReturnType ret;


    ret = FlashWriteEnable();
    UARTprintf("Flash write enable with ret: %d\n\r",ret);
    // Step 1: Disable Write protection
    if(ret == Flash_OperationTimeOut)
    {
        return Flash_OperationTimeOut;
    }

    char_stream_send.length   = 1;
    char_stream_send.pChar    = &cPER;

    Serialize_SPI(&char_stream_send,
                  NULL_PTR,
                  OpsWakeUp,
                  OpsEndTransfer);

#ifdef DEBUG
    printf("ENTERing 4-byte-addr mode\n");
#endif

    //ret = WAIT_TILL_Instruction_EXECUTION_COMPLETE(1);

    delayms(1000);

    /* verify current addr mode */
    //fdo->GenOp.ReadFlagStatusRegister(&flag);
    UARTprintf("FLAG:%d\n\r",flag);
    FlashReadFlagStatusRegister(&flag);
    UARTprintf("FLAG:%x\n\r",flag);


    if (flag & 1)
    {
        UARTprintf("Inside 4 byte address mode\n\r");
        fdo->Desc.NumAddrByte = FLASH_4_BYTE_ADDR_MODE;
    }
    else
    {
        UARTprintf("Inside 3 byte address mode\n\r");
        fdo->Desc.NumAddrByte = FLASH_3_BYTE_ADDR_MODE;
    }


    UARTprintf("EXIT FLASH 4BYTE MODE API with ret: %x\n",ret);




    //        ret = FlashReadNVConfigurationRegister(pIdentification);
    //         // UARTprintf("Enabling Quad Mode....\n\r");
    //          delayms(2000);
    //          UARTprintf("READ Enhanced Configuration Register = 0x%x  with ret:%d\n\r", pIdentification[0],ret);




#endif





#if 1//enable for while flash write/Disable while only reading



#if 1
    //--------------------------------------------------------------BULK ERASE--------------------------------------------------------------//

    uSectorType SectrPointer;
    ReturnType RetVal;
    SectrPointer = 0;

    UARTprintf("Entering Bulk erase.....\n\r");

    //RetVal = FlashSectorErase(SectrPointer);
    // RetVal = FlashBulkErase();

    //CharStream char_stream_send;
    uint8  cBE = SPI_FLASH_INS_BE;
    uint8 fsr_value;
    // ReturnType ret;

    // Step 1: Check whether any previous Write, Program or Erase cycle is on going
    if(IsFlashBusy()) return Flash_OperationOngoing;

    // Step 2: Disable Write protection
    if(FlashWriteEnable() == Flash_OperationTimeOut)
        return Flash_OperationTimeOut;

    // Step 3: Initialize the data(Instruction & address) packet to be sent serially
    char_stream_send.length   = 1;
    char_stream_send.pChar    = &cBE;

    // Step 4: Send the packet(Instruction & address) serially
    Serialize_SPI(&char_stream_send,
                  NULL_PTR,
                  OpsWakeUp,
                  OpsEndTransfer);

    // Step 5: Wait until the operation completes or a timeout occurs.
    //ret = WAIT_TILL_Instruction_EXECUTION_COMPLETE(BE_TIMEOUT);
    delaySec(10);

#ifdef FSR_SYNC
    FlashReadFlagStatusRegister(&fsr_value);
    FlashClearFlagStatusRegister();

    if((fsr_value & SPI_FSR_PROT) && (fsr_value & SPI_FSR_ERASE))
        return Flash_SectorProtected;
#endif




    //delayms(1000);//Enable if sector not working


    UARTprintf("Sector erase done with value %d\n",RetVal);







#endif

#if 0  //--------------------------------------------------READ MEMORY----------------------------------------------//


    memset(RecvBuff,0,sizeof(RecvBuff));

    ProgmPointer.Read.udAddr = FLASH_ADDR;
    ProgmPointer.Read.pArray = RecvBuff;
    ProgmPointer.Read.udNrOfElementsToRead = FLASH_BUFF_LEN;

    UARTprintf("ENTERING FLASH READ......");

    ret = DataRead(QuadOutputFastRead,&ProgmPointer);//For quad mode
    // DataRead(FourByteAddrPageRead,&ProgmPointer);//For 4 byte address mode
    UARTprintf("FLASH READ DONE AFTER ERASE with ret: %d..........!!",ret);


    for(z = 0; z < FLASH_BUFF_LEN;z++)
    {
        UARTprintf(" AFTER ERASE RecvBuff[%d] = %x\n",z,RecvBuff[z]);
    }


#endif
#if 0
    //------------------------------------------------PAGE PROGRAM---------------------------------------------//

    for(z = 0; z < FLASH_BUFF_LEN;z++)
    {
        pIdentification[z] = z;
        UARTprintf("SEND_BUFF[%d]: %x\n\r",z,pIdentification[z]);

    }

    UARTprintf("Enterring PAGE Program....");


    ProgmPointer.PageProgram.udAddr = FLASH_ADDR;
    ProgmPointer.PageProgram.pArray = &pIdentification;
    ProgmPointer.PageProgram.udNrOfElementsInArray = FLASH_BUFF_LEN;

    ret = DataProgram(QuadFastProgram4byte, &ProgmPointer);//For Quad mode
    // ret = DataProgram(FourByteAddrPageProgram, &ProgmPointer);//for 4 byte address mode


    UARTprintf("DELAY.....");
    //delayms(1);//Enable if page program not working

    UARTprintf("PAGE Programmed with ret: %d\n\r",ret);
    memset(pIdentification,0,sizeof(pIdentification));


    //--------------------------------------------------READ MEMORY----------------------------------------------//

    // uint8_t RecvBuff[300];
    memset(RecvBuff,0,sizeof(RecvBuff));

    UARTprintf("Enterring Read Memory....");

    ProgmPointer.Read.udAddr = FLASH_ADDR;
    ProgmPointer.Read.pArray = RecvBuff;
    ProgmPointer.Read.udNrOfElementsToRead = FLASH_BUFF_LEN + 10;

    UARTprintf("ENTERING FLASH READ");

    DataRead(QuadOutputFastRead,&ProgmPointer);//For quad mode

    //DataRead(FourByteAddrPageRead,&ProgmPointer);//For 4byte address mode
    //delayms(1);//Enable if Read memory not working
    UARTprintf("FLASH READ DONE with ret: %d..........!!",ret);


    for(z = 0; z <FLASH_BUFF_LEN+5;z++)
    {
        UARTprintf(" AFTER PAGE PROGRAM RecvBuff[%d] = %x\n",z,RecvBuff[z]);
    }


#endif

#endif

#endif


#ifdef IMU_SENSOR_

    u32I2CBase = I2C2_BASE;
    UARTprintf("Bno055 IMU Sensor Interface\n");
    UARTprintf("DELAY...\n\r");
    delaySec(1);
    u8RetVal = BNO055_Init(&rObj);
    UARTprintf("After bno055_assignI2C \r\n");

#endif

    /************************MCU External SRAM  - Module Initialization ***********************************/


#ifdef MCU_SDRAM_
    /************************MCU External SDRAM  - Module Initialization ***********************************/

    UARTprintf("SDRAM ReadWrite\n");

    u8RetVal = SDRAM_Init();

    /************************MCU External SDRAM  - Module Initialization ************************************/
#endif

    /************************MCU External SDRAM  - Module Initialization ************************************/

#ifdef MCU_FPGA_GPIO_

    /************MCU FPGA GPIO START ***********************************/

    // GPIO Pins set as output
    // u8RetVal = GPIO_Output_Init(SYSCTL_PERIPH_GPIOD,GPIO_PORTD_BASE,(GPIO_PIN_7),GPIO_PIN_7);

    // u8RetVal = GPIO_Output_Init(SYSCTL_PERIPH_GPIOD,GPIO_PORTD_BASE,(GPIO_PIN_7 | GPIO_PIN_6),GPIO_PIN_6);


    //GPIO Pins set as Input
    //  u8RetVal = GPIO_Input_Init(SYSCTL_PERIPH_GPIOD,GPIO_PORTD_BASE,GPIOD,(GPIO_PIN_7 | GPIO_PIN_6),INT_GPIOD,GPIO_PULLUP_ENABLE,GPIO_BOTH_EDGES,GPIO_INT_ENABLE);

    //GPIO Pins set as output
    u8RetVal = GPIO_Output_Init(SYSCTL_PERIPH_GPIOE,GPIO_PORTE_BASE,(GPIO_PIN_1 | GPIO_PIN_0),GPIO_PIN_1|GPIO_PIN_0); // GPIO_PIN_0 SPI Slave Mode , GPIO_PIN_1 RFIC Reset pin High

    //GPIO Pins set as Input
    //  u8RetVal = GPIO_Input_Init(SYSCTL_PERIPH_GPIOE,GPIO_PORTE_BASE,GPIOE,(GPIO_PIN_1 | GPIO_PIN_0),INT_GPIOE,GPIO_PULLUP_ENABLE,GPIO_BOTH_EDGES,GPIO_INT_ENABLE);

    //GPIO Pins set as output
    //    u8RetVal = GPIO_Output_Init(SYSCTL_PERIPH_GPIOK,GPIO_PORTK_BASE,(GPIO_PIN_1 | GPIO_PIN_2),GPIO_PIN_1);

    u8RetVal = GPIO_Output_Init(SYSCTL_PERIPH_GPIOK,GPIO_PORTK_BASE,(GPIO_PIN_1),GPIO_PIN_1);


    //GPIO Pins set as Input
    u8RetVal = GPIO_Input_Init(SYSCTL_PERIPH_GPIOK,GPIO_PORTK_BASE,GPIOK,(GPIO_PIN_2),INT_GPIOK,GPIO_PULLUP_ENABLE,GPIO_BOTH_EDGES,GPIO_INT_DISABLE);


    /************MCU FPGA GPIO END***********************************/

    //for FPGA ILA Clock PN3 and PQ4 should be high

    u8RetVal = GPIO_Output_Init(SYSCTL_PERIPH_GPION,GPIO_PORTN_BASE,(GPIO_PIN_3),GPIO_PIN_3); //

    u8RetVal = GPIO_Output_Init(SYSCTL_PERIPH_GPIOQ,GPIO_PORTQ_BASE,(GPIO_PIN_4),GPIO_PIN_4);


       ROM_GPIOPinWrite(GPIO_PORTN_BASE, GPIO_PIN_3,GPIO_PIN_3);

       ROM_GPIOPinWrite(GPIO_PORTQ_BASE, GPIO_PIN_4,GPIO_PIN_4);



     /* delaySec(3);

       ROM_GPIOPinWrite(GPIO_PORTN_BASE, GPIO_PIN_3,0);

       ROM_GPIOPinWrite(GPIO_PORTQ_BASE, GPIO_PIN_4,0);*/
#endif

#ifdef MCU_GPIO_LED

    /************MCU External LED  Green AND Red START***********************************/

    /*****MCU LED GREEN********/
    //GPIO Pins set as output
    u8RetVal = GPIO_Output_Init(SYSCTL_PERIPH_GPIOF,GPIO_PORTF_BASE,GPIO_PIN_3,GPIO_PIN_3);

    /*****MCU LED RED********/
    //GPIO Pins set as output
    u8RetVal = GPIO_Output_Init(SYSCTL_PERIPH_GPIOK,GPIO_PORTK_BASE,GPIO_PIN_6,GPIO_PIN_6);

#endif



#ifdef MCU_EXT_GPIO_


    //GPIO Pins set as output
    //u8RetVal = GPIO_Output_Init(SYSCTL_PERIPH_GPIOD,GPIO_PORTD_BASE,(GPIO_PIN_5 |GPIO_PIN_4),GPIO_PIN_LOW);

    u8RetVal = GPIO_Output_Init(SYSCTL_PERIPH_GPIOF,GPIO_PORTF_BASE,(GPIO_PIN_1 |GPIO_PIN_0),GPIO_PIN_LOW); //pin swapped in MSC Rev A from PORT D to PORT F


    //GPIO Pins set as Input
    // u8RetVal = GPIO_Input_Init(SYSCTL_PERIPH_GPIOD,GPIO_PORTD_BASE,GPIOD,(GPIO_PIN_5 |GPIO_PIN_4),INT_GPIOD,GPIO_PULLUP_ENABLE,GPIO_BOTH_EDGES,GPIO_INT_ENABLE);

    //GPIO Pins set as output
    u8RetVal = GPIO_Output_Init(SYSCTL_PERIPH_GPIOK,GPIO_PORTK_BASE,(GPIO_PIN_3),GPIO_PIN_LOW);

    //GPIO Pins set as Input
    //   u8RetVal = GPIO_Input_Init(SYSCTL_PERIPH_GPIOK,GPIO_PORTK_BASE,GPIOK,(GPIO_PIN_3),INT_GPIOH,GPIO_PULLUP_ENABLE,GPIO_BOTH_EDGES,GPIO_INT_ENABLE);

    //GPIO Pins set as output
    u8RetVal = GPIO_Output_Init(SYSCTL_PERIPH_GPIOL,GPIO_PORTL_BASE,(GPIO_PIN_5 | GPIO_PIN_4),GPIO_PIN_LOW);

    //GPIO Pins set as Input
    //  u8RetVal = GPIO_Input_Init(SYSCTL_PERIPH_GPIOL,GPIO_PORTL_BASE,GPIOL,(GPIO_PIN_5 | GPIO_PIN_4),INT_GPIOH,GPIO_PULLUP_ENABLE,GPIO_BOTH_EDGES,GPIO_INT_ENABLE);

    //GPIO Pins set as output
    //    u8RetVal = GPIO_Output_Init(SYSCTL_PERIPH_GPIOM,GPIO_PORTM_BASE,(GPIO_PIN_7 |GPIO_PIN_6 | GPIO_PIN_5 | GPIO_PIN_4),GPIO_PIN_LOW);

    //GPIO Pins set as Input
    u8RetVal = GPIO_Input_Init(SYSCTL_PERIPH_GPIOM,GPIO_PORTM_BASE,GPIOM,(GPIO_PIN_7 |GPIO_PIN_6 | GPIO_PIN_5 | GPIO_PIN_4),INT_GPIOM,GPIO_PULLUP_DISABLE,GPIO_BOTH_EDGES,GPIO_INT_DISABLE);

    //GPIO Pins set as output
    //    u8RetVal = GPIO_Output_Init(SYSCTL_PERIPH_GPION,GPIO_PORTN_BASE,(GPIO_PIN_2),GPIO_PIN_LOW);

    //GPIO Pins set as Input
    u8RetVal = GPIO_Input_Init(SYSCTL_PERIPH_GPION,GPIO_PORTN_BASE,GPION,(GPIO_PIN_2),INT_GPION,GPIO_PULLUP_DISABLE,GPIO_BOTH_EDGES,GPIO_INT_DISABLE);

    /************MCU External  GPIO END***********************************/


   /* ROM_GPIOPinWrite(GPIO_PORTF_BASE, GPIO_PIN_0,GPIO_PIN_0);
    ROM_GPIOPinWrite(GPIO_PORTF_BASE, GPIO_PIN_1,GPIO_PIN_1);
    ROM_GPIOPinWrite(GPIO_PORTK_BASE, GPIO_PIN_3,GPIO_PIN_3);
    ROM_GPIOPinWrite(GPIO_PORTL_BASE, GPIO_PIN_5,GPIO_PIN_5);
    ROM_GPIOPinWrite(GPIO_PORTL_BASE, GPIO_PIN_6,GPIO_PIN_6);
*/


#endif

#ifdef MCU_RFIC_
    /************************AD9364 RFIC - Module Initialization *****************************************/

    /********** GPIO RFIC CONFIGURARION SPI-0 ENABLE ******************/
    ROM_GPIOPinWrite(GPIO_PORTE_BASE, GPIO_PIN_0,0);

    u8RetVal =  AD9364_RFIC_Init();

    /********** GPIO RFIC CONFIGURARION SPI-0 DISABLE ******************/
    ROM_GPIOPinWrite(GPIO_PORTE_BASE, GPIO_PIN_0,GPIO_PIN_0);

#endif

#ifdef Timer_0
    /************************MCU Timer - Module Initialization *****************************************/


    /*Timer -0 Configuration */
    u32milliseconds = TIMER0_VAL_MS;
    u8RetVal = Timer0_Init(u32milliseconds);

#endif

#ifdef Timer_1_

    /*Timer -1 Configuration */
    u32milliseconds = TIMER1_VAL_MS;
    u8RetVal = Timer1_Init(u32milliseconds);
#endif

#ifdef  WDT_0_
    /************************MCU WDT - Module Initialization ********************************************************/

    /*Timer -0 Configuration */
    u32milliseconds = WDT0_VAL_MS;
    u8RetVal = WDT0_Init(u32milliseconds);
#endif

#ifdef  WDT_1_

    /*Timer -1 Configuration */
    u32milliseconds = WDT1_VAL_MS;
    u8RetVal = WDT1_Init(u32milliseconds);

#endif

    /*Reset Global Flag Variables*/
    g_ui32Flags = 0;
    g_ui32Flag_VC = 0;
    g_ui32TimerCount = 0;
    g_ui32Timer0Count = 0;

    UINT32  u32Index = 0;

    uint32_t getResponseData = 0;

    UINT32 u32DataIndex = 0;

    getResponseData = 0;

#ifdef SPI_0

    /************Reconfigure SPI-0 RFIC TX for 12 bit and 60MHZ**********/

    /****************SPI0 Configuration as Master***********/
    u32BitRate = 60000000;

    UARTprintf("SPI-0:u32BitRate:%d \n",u32BitRate);

    u32Mode = SSI_MODE_MASTER;

    u8RetVal = SPI0_Init(u32BitRate,SSI_MODE_MASTER,SSI_FRF_MOTO_MODE_0,12);

#endif

    /**********QUAD Mode Write and read Configuration********/

    // while(ROM_SSIDataGetNonBlocking(SSI2_BASE,&getResponseData));

    //  ROM_SSIAdvModeSet(SSI2_BASE,SSI_ADV_MODE_QUAD_WRITE);

    ROM_SSIAdvModeSet(SSI2_BASE,SSI_ADV_MODE_QUAD_READ);

#endif

    UINT32 i,j;UINT8 u8RxByte;


#if 1 // ethernet POE testing full duplex
    UARTprintf("...............DEAL ST ODU MCU START.................. \n");

    u32IntegrationDataFlag=0;

#if 1 //Power supply card test

    UINT32 EthData=0xDD;

    for (i=0;i<1024;i++)
    {
        gu8aDataBuff[i] = EthData;

        /*if(EthData == 256)
        {
            EthData =0;
        }*/
    }



    UARTprintf("Waiting to receive the Data\n");


#if 1


        gip_addr.addr = 0x1E01A8C0;
        gu16port = 5002;

        while(1)
        {
            Ethernet_Send(gpUDP_PCB, &gip_addr, gu16port, gu8aDataBuff,1024);
            delayms(1000);


        }
#endif

#endif

#endif

#if 0//Xmodem Receive for FPGA Boot



    UARTprintf("\nPlease transfer file from PC:\n", -1);

    //  memset(&gu8BootData[0],0,sizeof(gu8BootData));

    //       while (ROM_UARTCharsAvail(UART0_BASE))
    //        {
    //            ROM_UARTCharGetNonBlocking(UART0_BASE);
    //        }
    /********************X-Modem protocol START*********************************/
    if( 0 > xmodemReceive((unsigned char *)0x60000000,
                          BL_UART_MAX_IMAGE_SIZE))
    {
        UARTprintf("\nXmodem receive error\n", -1);
        u8RetVal = 0;
    }

#endif

#if 0
  /* memset(&pu8RdSdramDataBuff[0],0,sizeof(pu8RdSdramDataBuff));

    for(i = 0x00; i <= 1024 ; i++,j++) //13712  //emmc 40192
    {

        sdram8bitAddrPointer[i] = gu8BootData[i];

    }*/
   /* for(i = 0; i <= 20479; i++)//13712  //emmc 40192
       {

          // pu8RdSdramDataBuff[i] = sdram8bitAddrPointer[i];
          UARTprintf("SDRDATA[%d]:%x\n",i,pu8RdSdramDataBuff[i]);
       }*/



   // memset(&pu8RdSdramDataBuff[0],0,sizeof(pu8RdSdramDataBuff));



    for(i = 0; i <= 1024; i++)//13712  //emmc 40192
    {

        pu8RdSdramDataBuff[i] = sdram8bitAddrPointer[i];
       UARTprintf("SDRDATA[%d]:%x\n",i,pu8RdSdramDataBuff[i]);
    }

    UARTprintf("Receive Data Success in Xmodem\n");


    //CallApplication(SDRAM_APP_START_ADDRESS);

#endif



#if 0 //NAVIC START
    //*************************************************************************//
    /*getting GPS details*/
    u8RxByte = UARTCharGet(UART2_BASE);

    UARTprintf("%c",u8RxByte);

    //*************************************************************************//
#endif //NAVIC END

#if 0//Xmodem Receive for MCU BOOT



    UARTprintf("\nPlease transfer file from PC:\n", -1);

    /********************X-Modem protocol START*********************************/
    if( 0 > xmodemReceive((unsigned char *)0x00000000,
                          BL_UART_MAX_IMAGE_SIZE))
    {
        UARTprintf("\nXmodem receive error\n", -1);
        u8RetVal = 0;
    }

    //CallApplication(SDRAM_APP_START_ADDRESS);

#endif
    // UARTprintf("After XmodemReceive Data\n");

#if 0


    memset(&pu8RdSdramDataBuff[0],0,sizeof(pu8RdSdramDataBuff));

    SPI_Write_Read_App();

    j=5;
    for(i = 0x00; i <= 20479 ; i++,j++) //13712  //emmc 40192
    {

        sdram8bitAddrPointer[i] = pu8WrSdramDataBuff[j];

    }
    /* for(i = 0; i <= 20479; i++)//13712  //emmc 40192
       {

          // pu8RdSdramDataBuff[i] = sdram8bitAddrPointer[i];
          UARTprintf("SDRDATA[%d]:%x\n",i,pu8RdSdramDataBuff[i]);
       }*/



    // memset(&pu8RdSdramDataBuff[0],0,sizeof(pu8RdSdramDataBuff));



    for(i = 0; i <= 20479; i++)//13712  //emmc 40192
    {

        pu8RdSdramDataBuff[i] = sdram8bitAddrPointer[i];
        // UARTprintf("SDRDATA[%d]:%x\n",i,pu8RdSdramDataBuff[i]);
    }

    UARTprintf("Receive Data Success in Xmodem\n");


    CallApplication(SDRAM_APP_START_ADDRESS);

#endif




#if 0 //enable for FPGA boot flash write

#if 1//Xmodem Receive for FPGA Boot



    UARTprintf("\nPlease transfer file from PC:\n", -1);

    //  memset(&gu8BootData[0],0,sizeof(gu8BootData));

    //       while (ROM_UARTCharsAvail(UART0_BASE))
    //        {
    //            ROM_UARTCharGetNonBlocking(UART0_BASE);
    //        }
    /********************X-Modem protocol START*********************************/
    if( 0 > xmodemReceive((unsigned char *)0x60000000,
                          BL_UART_MAX_IMAGE_SIZE))
    {
        UARTprintf("\nXmodem receive error\n", -1);
        u8RetVal = 0;
    }

#endif

#if 1

    /* for(i = 0; i <=453632 ; i++) //13712  //emmc 40192//1192//453194 453632 //1219923
    {
        ROM_SSIDataPut(SSI1_BASE,sdram8bitAddrPointer[i]);
    }*/


    /* memset(&pu8RdSdramDataBuff[0],0,sizeof(pu8RdSdramDataBuff));

    for(i = 0x00; i <= 1024 ; i++,j++) //13712  //emmc 40192
    {

        sdram8bitAddrPointer[i] = gu8BootData[i];

    }*/
    /* for(i = 0; i <= 20479; i++)//13712  //emmc 40192
       {

          // pu8RdSdramDataBuff[i] = sdram8bitAddrPointer[i];
          UARTprintf("SDRDATA[%d]:%x\n",i,pu8RdSdramDataBuff[i]);
       }*/



    // memset(&pu8RdSdramDataBuff[0],0,sizeof(pu8RdSdramDataBuff));



#if 1

    UINT32 SdReadCount;

    UINT32 FlashReadCount;
    static UINT32 u32FlashAddUpdate,flashwrRdAddress;
    gStrSPIFlashCmdPkt.u32WrRdAddr = 0x00;
    gStrSPIFlashCmdPkt.u32FlashReadOffset = 0x00;
    UINT32 u32FlashStartingAddress;
    u32FlashStartingAddress = FLASH_STRATING_ADD;

    for(SdReadCount =0;SdReadCount <= ((gu32XmodemReceiveLength/1024)+1) ; SdReadCount ++) // 1192 for full atp bin //3351 for full mcs file
    {
        // UARTprintf("**************************SdReadCount***********************************************:%d\n",SdReadCount);
        gStrSPIFlashCmdPkt.u32FlashReadOffset = u32FlashStartingAddress  +(1024*SdReadCount);

        UARTprintf("Write adrress:%x\n",gStrSPIFlashCmdPkt.u32FlashReadOffset);
        memcpy(&pu8RdSdramDataBuff[0],&sdram8bitAddrPointer[u32FlashAddUpdate],1024);

        /* for(i = 0,flashwrRdAddress,u32FlashAddUpdate; i < 1024; i++,u32FlashAddUpdate++,flashwrRdAddress++) //13712  //emmc 40192//1192
        {
            pu8RdSdramDataBuff[u32FlashAddUpdate] = sdram8bitAddrPointer[u32FlashAddUpdate];

          //  UARTprintf("S[%d]:%x\n",u32FlashAddUpdate,pu8RdSdramDataBuff[u32FlashAddUpdate]);
          //  UARTprintf("FR[%d]:%x\n",flashwrRdAddress,pu8WrSdramDataBuff[u32FlashAddUpdate]);
            //ROM_SSIDataPut(SSI1_BASE,pu8WrSdramDataBuff[u32FlashAddUpdate]);
        }*/



        // UARTprintf("Write adrress:%x\n",gStrSPIFlashCmdPkt.u32FlashReadOffset);
        u8RetVal = SPI_NOR_Flash_Write(&Obj, gStrSPIFlashCmdPkt.u32FlashReadOffset,
                                       &pu8RdSdramDataBuff[0],
                                       1024);
        memset(&pu8RdSdramDataBuff[0],0,1024);

        u32FlashAddUpdate = u32FlashAddUpdate + 1024;
        //        gStrSPIFlashCmdPkt.u32FlashReadOffset = flashwrRdAddress;
    }


#endif


#endif




    /* for(i = 0; i <= 1219923; i++) //13712  //emmc 40192//1192
    {
        ROM_SSIDataPut(SSI1_BASE,sdram8bitAddrPointer[i]);
    }
     */



    UARTprintf("Receive Data Success in Xmodem\n");


    SPI_Write_Read_App();

#endif



#if 0

    for(i = 0x00; i <= 313875 ; i++) //13712  //emmc 40192
    {

        sdram8bitAddrPointer[i] = 0;

    }

    for(i = 0x00; i <= 1024 ; i++) //13712  //emmc 40192
    {

        UARTprintf("FD[%d]:%x\n",i,sdram8bitAddrPointer[i]);
    }


    SPI_Write_Read_App();

    UINT32 k;

#endif



#if 0


    for(i = 0; i <=gu32XmodemReceiveLength ; i++) //13712  //emmc 40192//1192//453194 453632 //1219923
    {
        ROM_SSIDataPut(SSI1_BASE,sdram8bitAddrPointer[i]);

    }


#endif




#if   0 //Xmodem Transmit
    int st;


    /*UINT32 u32Addr=0x00;

    UINT32 u32DataLength;
    u32DataLength = 5120;
    UINT32 BootCount;

    UINT8 BootData=65;
    for(BootCount=0;BootCount<u32DataLength;BootCount++)
    {
        gu8XmodemTransmitBuff[BootCount] = BootData++;
        if (BootData == 91)
        {
            BootData = 65;
        }
    }



    for(BootCount=0;BootCount<u32DataLength;BootCount++)
    {
        UARTprintf("BootData[%d]:%x\n",BootCount,gu8XmodemTransmitBuff[BootCount]);
    }
     */


    UARTprintf("Xmodem Test\n");

    printf ("Prepare your terminal emulator to receive data now...\n");
    /* the following should be changed for your environment:
               0x30000 is the download address,
               12000 is the maximum size to be send from this address
     */
    /*  if( 0 > xmodemTransmit((unsigned char *)0x60000000,
                                  BL_UART_MAX_IMAGE_SIZE))
            {
                UARTprintf("\nXmodem Transmit error\n", -1);
                u8RetVal = 0;
            }*/


    st =  xmodemTransmit(&pu8RdSdramDataBuff,40192);
    // if( 0 > xmodemTransmit((unsigned char *)0x60000000, BL_UART_MAX_IMAGE_SIZE))
    if (st < 0) {
        UARTprintf ("Xmodem transmit error: status: %d\n", st);
    }
    else  {
        UARTprintf ("Xmodem successfully transmitted %d bytes\n", st);
    }

    return 0;

#endif


    while (1)
    {
        printf("Inside Main while \n");


#ifdef  MCU_PROCESS_

        /*******Check for RFIC RX DATA Receive Flag**********/

        if(RFICDataReadFlag == FLAG_ENABLE)
        {
            /*******Check for RFIC FIFO PIN for 1K Sample**********/
            if(ROM_GPIOPinRead(GPIO_PORTK_BASE,GPIO_PIN_2))
            {
                /*******Read RFIX RX Sampel data using QSPI from FPGA**********/
                for(u32Index = 0; u32Index < 1536; u32Index++)
                {
                    ROM_SSIDataPut(SSI2_BASE,0X00);

                    ROM_SSIDataGet(SSI2_BASE,&u8aRFICDataBuff[u32DataIndex++]);
                }

                /*******Check for 2K RFIC Sample data**********/
                if(u32DataIndex == 3072)
                {
                    /***Reset Data Index*****/
                    u32DataIndex = 0;
                    AD9364_RFIC_Data_Receive();
                }
            }
            else
            {
                UARTprintf("....LOW....\n ");
            }
        }


        ROM_GPIOPinWrite(GPIO_PORTF_BASE, GPIO_PIN_3,0);

        /* Check for Ethernet RX Flag*/
        if(g_ui32Flags == FLAG_ENABLE)
        {
            /****Reset Ethernet data Flag******/
            g_ui32Flags = FLAG_RESET;

            printf("Inside command Handler \n");

            /****Process the RX Ethernet command ******/
            EthCmdHandler(gu8EthDataBuff,DataLength);

#if 1  // MCU AND FPGA BOOT TEST WITH GUI

            if(gStrBootFlashCmdPkt.u8CmdFlagStatus == CMD_FLAG_STOP &&  gStrEthCmdPkt.u8GPId == MCU_APPLICATION_BOOTTEST)
            {
                UARTprintf("Inside MCU Booting\n");
                int i,j;
                gStrSPIFlashCmdPkt.u32WrRdAddr = 0x00;

                u8RetVal = SPI_NOR_Flash_Read(&Obj,gStrSPIFlashCmdPkt.u32WrRdAddr,
                                              &pu8RdSdramDataBuff[0],
                                              19000);//20479

                j=5;
                for(i = 0; i <= 18672; i++,j++)//13712  //emmc 40192
                {

                    sdram8bitAddrPointer[i]= pu8RdSdramDataBuff[j];
                  //  UARTprintf("SDDATA[%d]:%x\n",i,sdram8bitAddrPointer[i]);

                }

                CallApplication(SDRAM_APP_START_ADDRESS);  // MCU Booting

            }

            if(gStrBootFlashCmdPkt.u8CmdFlagStatus == CMD_FLAG_STOP && gStrEthCmdPkt.u8GPId == FPGA_APPLICATION_BOOTTEST)
            {

                UARTprintf("Inside FPGA Booting\n");
                SPI_Write_Read_App(); //FPGA Booting

                for(i = 0; i <=313876 ; i++) //13712  //emmc 40192//1192//453194 453632 //1219923
                {
                    ROM_SSIDataPut(SSI1_BASE,sdram8bitAddrPointer[i]);

                }

            }

#endif  // MCU AND FPGA BOOT TEST WITH GUI ---END


        }

        ROM_GPIOPinWrite(GPIO_PORTF_BASE, GPIO_PIN_3,GPIO_PIN_3);
#endif

#ifdef WDT_INT_CLEAR
        //
        // Clear the watchdog interrupt.
        //
        MAP_WatchdogIntClear(WATCHDOG0_BASE);
#endif
    }

}



void CallApplication(uint_fast32_t ui32StartAddr)
{
    //
    // Set the vector table to the beginning of the app in SDRAM.
    //
    HWREG(NVIC_VTABLE) = ui32StartAddr;

#if 0
    __asm("    movw r0, #0x0000\n"
            "    movt r0, #0x6000\n"
            "    ldr  sp, [r0]\n"
            "    ldr  r0, [r0, #4]\n"
            "    bx   r0");
#endif
    //
    // Load the stack pointer from the application's vector table.
    // Load the initial PC from the application's vector table and branch to
    // the application's entry point.
    //
    __asm("    ldr     sp, [r0]\n"
            "    ldr     r0, [r0, #4]\n"
            "    bx      r0\n");
}




void SPI_Write_Read_App()
{

    UINT32 u8RetVal,i;   UINT32 FlashReadCount;
    static UINT32 flashwrRdAddress,SdramAddress;
    static UINT32 u32FlashAddUpdate;
    gStrSPIFlashCmdPkt.u32FlashReadOffset = 0;
    gStrSPIFlashCmdPkt.u32WrRdAddr = 0x00010000;
    UINT32 u32FlashStartingAddress;
    u32FlashStartingAddress = FLASH_STRATING_ADD;

    /* u8RetVal = SPI_NOR_Flash_Write(&Obj, gStrSPIFlashCmdPkt.u32FlashReadOffset,
                                   &gu8BootData[0],
                                   20479);//20479*/


    /*u8RetVal = SPI_NOR_Flash_Read(&Obj,gStrSPIFlashCmdPkt.u32WrRdAddr,
                                  &pu8WrSdramDataBuff[0],
                                  1024);//20479*/

    /*for(i = 0; i <= 1024; i++)//20479
    {

        UARTprintf("FRD[%d]:%x\n",i,pu8WrSdramDataBuff[i]);
    }
     */



    int flashReadData; int count,j;

    for( FlashReadCount = 0; FlashReadCount <= 307 ; FlashReadCount++ )/// 1192 for full atp bin //3351 for full mcs file
    {

        gStrSPIFlashCmdPkt.u32WrRdAddr = u32FlashStartingAddress +(1024*FlashReadCount);

        u8RetVal = SPI_NOR_Flash_Read(&Obj,gStrSPIFlashCmdPkt.u32WrRdAddr,
                                      &pu8WrSdramDataBuff[0],
                                      1024+5);


        j=5;
        for(flashReadData =0,count=0; flashReadData<1024 ; flashReadData ++,j++,count++)
        {

            sdram8bitAddrPointer[SdramAddress] = pu8WrSdramDataBuff[j];
            // ROM_SSIDataPut(SSI1_BASE,pu8WrSdramDataBuff[j]);
            //  UARTprintf("FD[%d]:%c\n",SdramAddress,pu8WrSdramDataBuff[j]);

            SdramAddress ++;
        }

        // UARTprintf("flashwrrdAdd:%d\n",gStrSPIFlashCmdPkt.u32WrRdAddr);


        memset(&pu8WrSdramDataBuff[0],0,1024);

        /*u32FlashAddUpdate = 5;
            for(i = 0,flashwrRdAddress,u32FlashAddUpdate; i < 1024; i++,u32FlashAddUpdate++,flashwrRdAddress++) //13712  //emmc 40192//1192
            {

                UARTprintf("FR[%d]:%x\n",flashwrRdAddress,pu8WrSdramDataBuff[u32FlashAddUpdate]);
                //ROM_SSIDataPut(SSI1_BASE,pu8WrSdramDataBuff[u32FlashAddUpdate]);
            }


            UARTprintf("count:%d\n",u32FlashAddUpdate);
            UARTprintf("flashwrrdAdd:%d\n",gStrSPIFlashCmdPkt.u32WrRdAddr);
            gStrSPIFlashCmdPkt.u32WrRdAddr = flashwrRdAddress;
            UARTprintf("flashwrrdAdd:%d\n",gStrSPIFlashCmdPkt.u32WrRdAddr);*/





    }

    UARTprintf("SDRAMcount:%d\n",SdramAddress);

    /*
        for(i = 0; i <=SdramAddress ; i++) //13712  //emmc 40192//1192//453194 453632 //1219923
        {
            ROM_SSIDataPut(SSI1_BASE,sdram8bitAddrPointer[i]);
        }*/

}





