/* USER CODE BEGIN Header */
/**
 ******************************************************************************
 * @file           : main.c
 * @brief          : Main program body
 ******************************************************************************
 * @attention
 *
 * Copyright (c) 2024 STMicroelectronics.
 * All rights reserved.
 *
 * This software is licensed under terms that can be found in the LICENSE file
 * in the root directory of this software component.
 * If no LICENSE file comes with this software, it is provided AS-IS.
 *
 ******************************************************************************
 */
/* USER CODE END Header */
/* Includes ------------------------------------------------------------------*/
#include "main.h"

/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */
#include <stdio.h>
#include <string.h>
#include "ATP_Mode.h"
#include "SRAM_Operations.h"
/* USER CODE END Includes */

/* Private typedef -----------------------------------------------------------*/
/* USER CODE BEGIN PTD */

/* USER CODE END PTD */

/* Private define ------------------------------------------------------------*/
/* USER CODE BEGIN PD */

/* USER CODE END PD */

/* Private macro -------------------------------------------------------------*/
/* USER CODE BEGIN PM */

/* USER CODE END PM */

/* Private variables ---------------------------------------------------------*/

SPI_HandleTypeDef hspi2;
SPI_HandleTypeDef hspi3;
SPI_HandleTypeDef hspi5;

UART_HandleTypeDef huart4;
UART_HandleTypeDef huart5;
UART_HandleTypeDef huart8;
UART_HandleTypeDef huart10;

SRAM_HandleTypeDef hsram1;
NAND_HandleTypeDef hnand1;

/* USER CODE BEGIN PV */


//const uint8_t WREG = 0x40; //00000011;
//const uint8_t RREG = 0x20; //00000011;
//const uint8_t RDATA = 0x12; //00000011;


extern uint8_t time_str[25];
uint32_t sramAddr ;
extern uint8_t pSrcBuffer ;
uint8_t read_flag = 0;
/* USER CODE END PV */

/* Private function prototypes -----------------------------------------------*/
void SystemClock_Config(void);
static void MX_GPIO_Init(void);
static void MX_FMC_Init(void);
static void MX_SPI2_Init(void);
static void MX_SPI3_Init(void);
static void MX_SPI5_Init(void);
static void MX_UART4_Init(void);
static void MX_UART5_Init(void);
static void MX_UART8_Init(void);
static void MX_USART10_UART_Init(void);
static void MX_ICACHE_Init(void);
/* USER CODE BEGIN PFP */

void UART_InitAllHandlers(void) ;
/* USER CODE END PFP */

/* Private user code ---------------------------------------------------------*/
/* USER CODE BEGIN 0 */

#define BufferSize 1

extern uint8_t buffer2[25];

uint8_t time_str[25]={0};
uint8_t time_str_length = 0 ;

extern uint8_t command_str[15];
extern uint8_t command_str_index;
uint8_t command_rcv ;
extern uint8_t ATP_test ;

uint8_t ADC_config_try=0;
uint8_t uart1_rx_buffer[1];
uint8_t uart2_rx_buffer[1];
uint8_t uart5_rx_buffer;
uint8_t rcv;
uint8_t time_rcv=0;
uint8_t time_set = 0;
uint8_t uart3_rx_buffer[1];
uint8_t uart4_rx_buffer[1];
extern NAND_AddressTypeDef nand_address;
/*
 * variables declaration for NAND Flash
 */
uint8_t NAND_Buf[15]={0};
uint8_t NAND_index = 0;
uint8_t NAND_flag=0;

_sSRAM_LastAddr update_loc;
/* USER CODE END 0 */

/**
 * @brief  The application entry point.
 * @retval int
 */
int main(void)
{

	/* USER CODE BEGIN 1 */


	/* USER CODE END 1 */

	/* MCU Configuration--------------------------------------------------------*/

	/* Reset of all peripherals, Initializes the Flash interface and the Systick. */
	HAL_Init();

	/* USER CODE BEGIN Init */

	/* USER CODE END Init */

	/* Configure the system clock */
	SystemClock_Config();

	/* USER CODE BEGIN SysInit */

	/* USER CODE END SysInit */

	/* Initialize all configured peripherals */
	MX_GPIO_Init();
	MX_FMC_Init();
	MX_SPI2_Init();
	MX_SPI3_Init();
	MX_SPI5_Init();
	MX_UART4_Init();
	MX_UART5_Init();
	MX_UART8_Init();
	MX_USART10_UART_Init();
	MX_ICACHE_Init();
	/* USER CODE BEGIN 2 */

	UART_InitAllHandlers();
	/* USER CODE END 2 */

	/* Infinite loop */
	/* USER CODE BEGIN WHILE */
	sramAddr = 0x60000000 | 0x7FFF8;
	pSrcBuffer = 0x00;
	HAL_SRAM_Write_8b(&hsram1,(uint32_t *)sramAddr,&pSrcBuffer,BufferSize);
	HAL_NAND_Reset(&hnand1);
	get_NAND_block_status();
	get_NAND_address();
	get_NAND_CyclicAddr();
	sram_location = get_SRAM_Addr();

	uint8_t SRAM_clean[8192] = {0};
	HAL_GPIO_WritePin(GPIOA, RS485_CTRL2_Pin, GPIO_PIN_RESET);
	while (1)
	{
#ifdef FUll_NAND_Er_
		nand_address.Block=0;
		while (1){

			HAL_NAND_Erase_Block(&hnand1, &nand_address);
			HAL_Delay(10);
			nand_address.Block++;

			if (nand_address.Block > MAX_BLOCK)
			{
				// Clear the SRAM location
				HAL_SRAM_Write_8b(&hsram1, (uint32_t *)sramAddr_base_addr, SRAM_clean, 8192);
				HAL_Delay(1000);
				nand_cyclic_run = 0;
				update_NAND_CyclicAddr();
				Init_NAND_addr();
				HAL_UART_Transmit(&huart5, "NAND ERASE DONE \n\r", strlen("NAND ERASE DONE \n\r"),HAL_MAX_DELAY);
				while (1);
			}

		}
#endif

#if 1
		while((HAL_GPIO_ReadPin(GPIOE,GPI_3_Pin) == 0 && HAL_GPIO_ReadPin(GPIOD,GPI_2_Pin) == 0)) // change condition when GPIOs input add
		{
			ADC_operation();
		}

//#else
		while((HAL_GPIO_ReadPin(GPIOE,GPI_3_Pin) == 0 && HAL_GPIO_ReadPin(GPIOD,GPI_2_Pin) == 1)) // change condition when GPIOs input add
		{
			ATP_operation();

			if(time_set)
			{
				time_set = 0 ;
				time_setter();

			}
		}
#endif
		/* USER CODE END WHILE */

		/* USER CODE BEGIN 3 */
	}
	/* USER CODE END 3 */
}

/**
 * @brief System Clock Configuration
 * @retval None
 */
void SystemClock_Config(void)
{
	RCC_OscInitTypeDef RCC_OscInitStruct = {0};
	RCC_ClkInitTypeDef RCC_ClkInitStruct = {0};

	/** Configure the main internal regulator output voltage
	 */
	__HAL_PWR_VOLTAGESCALING_CONFIG(PWR_REGULATOR_VOLTAGE_SCALE0);

	while(!__HAL_PWR_GET_FLAG(PWR_FLAG_VOSRDY)) {}

	/** Initializes the RCC Oscillators according to the specified parameters
	 * in the RCC_OscInitTypeDef structure.
	 */
	RCC_OscInitStruct.OscillatorType = RCC_OSCILLATORTYPE_HSE;
	RCC_OscInitStruct.HSEState = RCC_HSE_ON;
	RCC_OscInitStruct.PLL.PLLState = RCC_PLL_ON;
	RCC_OscInitStruct.PLL.PLLSource = RCC_PLL1_SOURCE_HSE;
	RCC_OscInitStruct.PLL.PLLM = 4;
	RCC_OscInitStruct.PLL.PLLN = 80;
	RCC_OscInitStruct.PLL.PLLP = 2;
	RCC_OscInitStruct.PLL.PLLQ = 2;
	RCC_OscInitStruct.PLL.PLLR = 2;
	RCC_OscInitStruct.PLL.PLLRGE = RCC_PLL1_VCIRANGE_2;
	RCC_OscInitStruct.PLL.PLLVCOSEL = RCC_PLL1_VCORANGE_WIDE;
	RCC_OscInitStruct.PLL.PLLFRACN = 0;
	if (HAL_RCC_OscConfig(&RCC_OscInitStruct) != HAL_OK)
	{
		Error_Handler();
	}

	/** Initializes the CPU, AHB and APB buses clocks
	 */
	RCC_ClkInitStruct.ClockType = RCC_CLOCKTYPE_HCLK|RCC_CLOCKTYPE_SYSCLK
			|RCC_CLOCKTYPE_PCLK1|RCC_CLOCKTYPE_PCLK2
			|RCC_CLOCKTYPE_PCLK3;
	RCC_ClkInitStruct.SYSCLKSource = RCC_SYSCLKSOURCE_PLLCLK;
	RCC_ClkInitStruct.AHBCLKDivider = RCC_SYSCLK_DIV1;
	RCC_ClkInitStruct.APB1CLKDivider = RCC_HCLK_DIV1;
	RCC_ClkInitStruct.APB2CLKDivider = RCC_HCLK_DIV1;
	RCC_ClkInitStruct.APB3CLKDivider = RCC_HCLK_DIV1;

	if (HAL_RCC_ClockConfig(&RCC_ClkInitStruct, FLASH_LATENCY_5) != HAL_OK)
	{
		Error_Handler();
	}

	/** Configure the programming delay
	 */
	__HAL_FLASH_SET_PROGRAM_DELAY(FLASH_PROGRAMMING_DELAY_2);
}

/**
 * @brief ICACHE Initialization Function
 * @param None
 * @retval None
 */
static void MX_ICACHE_Init(void)
{

	/* USER CODE BEGIN ICACHE_Init 0 */

	/* USER CODE END ICACHE_Init 0 */

	/* USER CODE BEGIN ICACHE_Init 1 */

	/* USER CODE END ICACHE_Init 1 */

	/** Enable instruction cache in 1-way (direct mapped cache)
	 */
	if (HAL_ICACHE_ConfigAssociativityMode(ICACHE_1WAY) != HAL_OK)
	{
		Error_Handler();
	}
	if (HAL_ICACHE_Enable() != HAL_OK)
	{
		Error_Handler();
	}
	/* USER CODE BEGIN ICACHE_Init 2 */

	/* USER CODE END ICACHE_Init 2 */

}

/**
 * @brief SPI2 Initialization Function
 * @param None
 * @retval None
 */
static void MX_SPI2_Init(void)
{

	/* USER CODE BEGIN SPI2_Init 0 */

	/* USER CODE END SPI2_Init 0 */

	/* USER CODE BEGIN SPI2_Init 1 */

	/* USER CODE END SPI2_Init 1 */
	/* SPI2 parameter configuration*/
	hspi2.Instance = SPI2;
	hspi2.Init.Mode = SPI_MODE_MASTER;
	hspi2.Init.Direction = SPI_DIRECTION_2LINES;
	hspi2.Init.DataSize = SPI_DATASIZE_8BIT;
	hspi2.Init.CLKPolarity = SPI_POLARITY_LOW;
	hspi2.Init.CLKPhase = SPI_PHASE_2EDGE;
	hspi2.Init.NSS = SPI_NSS_SOFT;
	hspi2.Init.BaudRatePrescaler = SPI_BAUDRATEPRESCALER_64;
	hspi2.Init.FirstBit = SPI_FIRSTBIT_MSB;
	hspi2.Init.TIMode = SPI_TIMODE_DISABLE;
	hspi2.Init.CRCCalculation = SPI_CRCCALCULATION_DISABLE;
	hspi2.Init.CRCPolynomial = 0x7;
	hspi2.Init.NSSPMode = SPI_NSS_PULSE_DISABLE;
	hspi2.Init.NSSPolarity = SPI_NSS_POLARITY_LOW;
	hspi2.Init.FifoThreshold = SPI_FIFO_THRESHOLD_01DATA;
	hspi2.Init.MasterSSIdleness = SPI_MASTER_SS_IDLENESS_00CYCLE;
	hspi2.Init.MasterInterDataIdleness = SPI_MASTER_INTERDATA_IDLENESS_00CYCLE;
	hspi2.Init.MasterReceiverAutoSusp = SPI_MASTER_RX_AUTOSUSP_DISABLE;
	hspi2.Init.MasterKeepIOState = SPI_MASTER_KEEP_IO_STATE_DISABLE;
	hspi2.Init.IOSwap = SPI_IO_SWAP_DISABLE;
	hspi2.Init.ReadyMasterManagement = SPI_RDY_MASTER_MANAGEMENT_INTERNALLY;
	hspi2.Init.ReadyPolarity = SPI_RDY_POLARITY_HIGH;
	if (HAL_SPI_Init(&hspi2) != HAL_OK)
	{
		Error_Handler();
	}
	/* USER CODE BEGIN SPI2_Init 2 */

	/* USER CODE END SPI2_Init 2 */

}

/**
 * @brief SPI3 Initialization Function
 * @param None
 * @retval None
 */
static void MX_SPI3_Init(void)
{

	/* USER CODE BEGIN SPI3_Init 0 */

	/* USER CODE END SPI3_Init 0 */

	/* USER CODE BEGIN SPI3_Init 1 */

	/* USER CODE END SPI3_Init 1 */
	/* SPI3 parameter configuration*/
	hspi3.Instance = SPI3;
	hspi3.Init.Mode = SPI_MODE_MASTER;
	hspi3.Init.Direction = SPI_DIRECTION_2LINES;
	hspi3.Init.DataSize = SPI_DATASIZE_8BIT;
	hspi3.Init.CLKPolarity = SPI_POLARITY_LOW;
	hspi3.Init.CLKPhase = SPI_PHASE_2EDGE;
	hspi3.Init.NSS = SPI_NSS_SOFT;
	hspi3.Init.BaudRatePrescaler = SPI_BAUDRATEPRESCALER_64;
	hspi3.Init.FirstBit = SPI_FIRSTBIT_MSB;
	hspi3.Init.TIMode = SPI_TIMODE_DISABLE;
	hspi3.Init.CRCCalculation = SPI_CRCCALCULATION_DISABLE;
	hspi3.Init.CRCPolynomial = 0x7;
	hspi3.Init.NSSPMode = SPI_NSS_PULSE_DISABLE;
	hspi3.Init.NSSPolarity = SPI_NSS_POLARITY_LOW;
	hspi3.Init.FifoThreshold = SPI_FIFO_THRESHOLD_01DATA;
	hspi3.Init.MasterSSIdleness = SPI_MASTER_SS_IDLENESS_00CYCLE;
	hspi3.Init.MasterInterDataIdleness = SPI_MASTER_INTERDATA_IDLENESS_00CYCLE;
	hspi3.Init.MasterReceiverAutoSusp = SPI_MASTER_RX_AUTOSUSP_DISABLE;
	hspi3.Init.MasterKeepIOState = SPI_MASTER_KEEP_IO_STATE_DISABLE;
	hspi3.Init.IOSwap = SPI_IO_SWAP_DISABLE;
	hspi3.Init.ReadyMasterManagement = SPI_RDY_MASTER_MANAGEMENT_INTERNALLY;
	hspi3.Init.ReadyPolarity = SPI_RDY_POLARITY_HIGH;
	if (HAL_SPI_Init(&hspi3) != HAL_OK)
	{
		Error_Handler();
	}
	/* USER CODE BEGIN SPI3_Init 2 */

	/* USER CODE END SPI3_Init 2 */

}

/**
 * @brief SPI5 Initialization Function
 * @param None
 * @retval None
 */
static void MX_SPI5_Init(void)
{

	/* USER CODE BEGIN SPI5_Init 0 */

	/* USER CODE END SPI5_Init 0 */

	/* USER CODE BEGIN SPI5_Init 1 */

	/* USER CODE END SPI5_Init 1 */
	/* SPI5 parameter configuration*/
	hspi5.Instance = SPI5;
	hspi5.Init.Mode = SPI_MODE_MASTER;
	hspi5.Init.Direction = SPI_DIRECTION_2LINES;
	hspi5.Init.DataSize = SPI_DATASIZE_8BIT;
	hspi5.Init.CLKPolarity = SPI_POLARITY_LOW;
	hspi5.Init.CLKPhase = SPI_PHASE_2EDGE;
	hspi5.Init.NSS = SPI_NSS_SOFT;
	hspi5.Init.BaudRatePrescaler = SPI_BAUDRATEPRESCALER_64;
	hspi5.Init.FirstBit = SPI_FIRSTBIT_MSB;
	hspi5.Init.TIMode = SPI_TIMODE_DISABLE;
	hspi5.Init.CRCCalculation = SPI_CRCCALCULATION_DISABLE;
	hspi5.Init.CRCPolynomial = 0x7;
	hspi5.Init.NSSPMode = SPI_NSS_PULSE_DISABLE;
	hspi5.Init.NSSPolarity = SPI_NSS_POLARITY_LOW;
	hspi5.Init.FifoThreshold = SPI_FIFO_THRESHOLD_01DATA;
	hspi5.Init.MasterSSIdleness = SPI_MASTER_SS_IDLENESS_00CYCLE;
	hspi5.Init.MasterInterDataIdleness = SPI_MASTER_INTERDATA_IDLENESS_00CYCLE;
	hspi5.Init.MasterReceiverAutoSusp = SPI_MASTER_RX_AUTOSUSP_DISABLE;
	hspi5.Init.MasterKeepIOState = SPI_MASTER_KEEP_IO_STATE_DISABLE;
	hspi5.Init.IOSwap = SPI_IO_SWAP_DISABLE;
	hspi5.Init.ReadyMasterManagement = SPI_RDY_MASTER_MANAGEMENT_INTERNALLY;
	hspi5.Init.ReadyPolarity = SPI_RDY_POLARITY_HIGH;
	if (HAL_SPI_Init(&hspi5) != HAL_OK)
	{
		Error_Handler();
	}
	/* USER CODE BEGIN SPI5_Init 2 */

	/* USER CODE END SPI5_Init 2 */

}

/**
 * @brief UART4 Initialization Function
 * @param None
 * @retval None
 */
static void MX_UART4_Init(void)
{

	/* USER CODE BEGIN UART4_Init 0 */

	/* USER CODE END UART4_Init 0 */

	/* USER CODE BEGIN UART4_Init 1 */

	/* USER CODE END UART4_Init 1 */
	huart4.Instance = UART4;
	huart4.Init.BaudRate = 115200;
	huart4.Init.WordLength = UART_WORDLENGTH_8B;
	huart4.Init.StopBits = UART_STOPBITS_1;
	huart4.Init.Parity = UART_PARITY_NONE;
	huart4.Init.Mode = UART_MODE_TX_RX;
	huart4.Init.HwFlowCtl = UART_HWCONTROL_NONE;
	huart4.Init.OverSampling = UART_OVERSAMPLING_16;
	huart4.Init.OneBitSampling = UART_ONE_BIT_SAMPLE_DISABLE;
	huart4.Init.ClockPrescaler = UART_PRESCALER_DIV1;
	huart4.AdvancedInit.AdvFeatureInit = UART_ADVFEATURE_NO_INIT;
	if (HAL_UART_Init(&huart4) != HAL_OK)
	{
		Error_Handler();
	}
	if (HAL_UARTEx_SetTxFifoThreshold(&huart4, UART_TXFIFO_THRESHOLD_1_8) != HAL_OK)
	{
		Error_Handler();
	}
	if (HAL_UARTEx_SetRxFifoThreshold(&huart4, UART_RXFIFO_THRESHOLD_1_8) != HAL_OK)
	{
		Error_Handler();
	}
	if (HAL_UARTEx_DisableFifoMode(&huart4) != HAL_OK)
	{
		Error_Handler();
	}
	/* USER CODE BEGIN UART4_Init 2 */

	/* USER CODE END UART4_Init 2 */

}

/**
 * @brief UART5 Initialization Function
 * @param None
 * @retval None
 */
static void MX_UART5_Init(void)
{

	/* USER CODE BEGIN UART5_Init 0 */

	/* USER CODE END UART5_Init 0 */

	/* USER CODE BEGIN UART5_Init 1 */

	/* USER CODE END UART5_Init 1 */
	huart5.Instance = UART5;
	huart5.Init.BaudRate = 115200;
	huart5.Init.WordLength = UART_WORDLENGTH_8B;
	huart5.Init.StopBits = UART_STOPBITS_1;
	huart5.Init.Parity = UART_PARITY_NONE;
	huart5.Init.Mode = UART_MODE_TX_RX;
	huart5.Init.HwFlowCtl = UART_HWCONTROL_NONE;
	huart5.Init.OverSampling = UART_OVERSAMPLING_16;
	huart5.Init.OneBitSampling = UART_ONE_BIT_SAMPLE_DISABLE;
	huart5.Init.ClockPrescaler = UART_PRESCALER_DIV1;
	huart5.AdvancedInit.AdvFeatureInit = UART_ADVFEATURE_NO_INIT;
	if (HAL_UART_Init(&huart5) != HAL_OK)
	{
		Error_Handler();
	}
	if (HAL_UARTEx_SetTxFifoThreshold(&huart5, UART_TXFIFO_THRESHOLD_1_8) != HAL_OK)
	{
		Error_Handler();
	}
	if (HAL_UARTEx_SetRxFifoThreshold(&huart5, UART_RXFIFO_THRESHOLD_1_8) != HAL_OK)
	{
		Error_Handler();
	}
	if (HAL_UARTEx_DisableFifoMode(&huart5) != HAL_OK)
	{
		Error_Handler();
	}
	/* USER CODE BEGIN UART5_Init 2 */

	/* USER CODE END UART5_Init 2 */

}

/**
 * @brief UART8 Initialization Function
 * @param None
 * @retval None
 */
static void MX_UART8_Init(void)
{

	/* USER CODE BEGIN UART8_Init 0 */

	/* USER CODE END UART8_Init 0 */

	/* USER CODE BEGIN UART8_Init 1 */

	/* USER CODE END UART8_Init 1 */
	huart8.Instance = UART8;
	huart8.Init.BaudRate = 115200;
	huart8.Init.WordLength = UART_WORDLENGTH_8B;
	huart8.Init.StopBits = UART_STOPBITS_1;
	huart8.Init.Parity = UART_PARITY_NONE;
	huart8.Init.Mode = UART_MODE_TX_RX;
	huart8.Init.HwFlowCtl = UART_HWCONTROL_NONE;
	huart8.Init.OverSampling = UART_OVERSAMPLING_16;
	huart8.Init.OneBitSampling = UART_ONE_BIT_SAMPLE_DISABLE;
	huart8.Init.ClockPrescaler = UART_PRESCALER_DIV1;
	huart8.AdvancedInit.AdvFeatureInit = UART_ADVFEATURE_NO_INIT;
	if (HAL_UART_Init(&huart8) != HAL_OK)
	{
		Error_Handler();
	}
	if (HAL_UARTEx_SetTxFifoThreshold(&huart8, UART_TXFIFO_THRESHOLD_8_8) != HAL_OK)
	{
		Error_Handler();
	}
	if (HAL_UARTEx_SetRxFifoThreshold(&huart8, UART_RXFIFO_THRESHOLD_1_8) != HAL_OK)
	{
		Error_Handler();
	}
	if (HAL_UARTEx_DisableFifoMode(&huart8) != HAL_OK)
	{
		Error_Handler();
	}
	/* USER CODE BEGIN UART8_Init 2 */

	/* USER CODE END UART8_Init 2 */

}

/**
 * @brief USART10 Initialization Function
 * @param None
 * @retval None
 */
static void MX_USART10_UART_Init(void)
{

	/* USER CODE BEGIN USART10_Init 0 */

	/* USER CODE END USART10_Init 0 */

	/* USER CODE BEGIN USART10_Init 1 */

	/* USER CODE END USART10_Init 1 */
	huart10.Instance = USART10;
	huart10.Init.BaudRate = 115200;
	huart10.Init.WordLength = UART_WORDLENGTH_8B;
	huart10.Init.StopBits = UART_STOPBITS_1;
	huart10.Init.Parity = UART_PARITY_NONE;
	huart10.Init.Mode = UART_MODE_TX_RX;
	huart10.Init.HwFlowCtl = UART_HWCONTROL_NONE;
	huart10.Init.OverSampling = UART_OVERSAMPLING_16;
	huart10.Init.OneBitSampling = UART_ONE_BIT_SAMPLE_DISABLE;
	huart10.Init.ClockPrescaler = UART_PRESCALER_DIV1;
	huart10.AdvancedInit.AdvFeatureInit = UART_ADVFEATURE_NO_INIT;
	if (HAL_UART_Init(&huart10) != HAL_OK)
	{
		Error_Handler();
	}
	if (HAL_UARTEx_SetTxFifoThreshold(&huart10, UART_TXFIFO_THRESHOLD_1_8) != HAL_OK)
	{
		Error_Handler();
	}
	if (HAL_UARTEx_SetRxFifoThreshold(&huart10, UART_RXFIFO_THRESHOLD_1_8) != HAL_OK)
	{
		Error_Handler();
	}
	if (HAL_UARTEx_DisableFifoMode(&huart10) != HAL_OK)
	{
		Error_Handler();
	}
	/* USER CODE BEGIN USART10_Init 2 */

	/* USER CODE END USART10_Init 2 */

}

/* FMC initialization function */
static void MX_FMC_Init(void)
{

	/* USER CODE BEGIN FMC_Init 0 */

	/* USER CODE END FMC_Init 0 */

	FMC_NORSRAM_TimingTypeDef Timing = {0};
	FMC_NAND_PCC_TimingTypeDef ComSpaceTiming = {0};
	FMC_NAND_PCC_TimingTypeDef AttSpaceTiming = {0};

	/* USER CODE BEGIN FMC_Init 1 */

	/* USER CODE END FMC_Init 1 */

	/** Perform the SRAM1 memory initialization sequence
	 */
	hsram1.Instance = FMC_NORSRAM_DEVICE;
	hsram1.Extended = FMC_NORSRAM_EXTENDED_DEVICE;
	/* hsram1.Init */
	hsram1.Init.NSBank = FMC_NORSRAM_BANK1;
	hsram1.Init.DataAddressMux = FMC_DATA_ADDRESS_MUX_DISABLE;
	hsram1.Init.MemoryType = FMC_MEMORY_TYPE_SRAM;
	hsram1.Init.MemoryDataWidth = FMC_NORSRAM_MEM_BUS_WIDTH_8;
	hsram1.Init.BurstAccessMode = FMC_BURST_ACCESS_MODE_DISABLE;
	hsram1.Init.WaitSignalPolarity = FMC_WAIT_SIGNAL_POLARITY_LOW;
	hsram1.Init.WaitSignalActive = FMC_WAIT_TIMING_BEFORE_WS;
	hsram1.Init.WriteOperation = FMC_WRITE_OPERATION_ENABLE;
	hsram1.Init.WaitSignal = FMC_WAIT_SIGNAL_DISABLE;
	hsram1.Init.ExtendedMode = FMC_EXTENDED_MODE_DISABLE;
	hsram1.Init.AsynchronousWait = FMC_ASYNCHRONOUS_WAIT_DISABLE;
	hsram1.Init.WriteBurst = FMC_WRITE_BURST_DISABLE;
	hsram1.Init.ContinuousClock = FMC_CONTINUOUS_CLOCK_SYNC_ONLY;
	hsram1.Init.WriteFifo = FMC_WRITE_FIFO_ENABLE;
	hsram1.Init.NBLSetupTime = 0;
	hsram1.Init.PageSize = FMC_PAGE_SIZE_NONE;
	hsram1.Init.MaxChipSelectPulse = DISABLE;
	/* Timing */
	Timing.AddressSetupTime = 15;
	Timing.AddressHoldTime = 15;
	Timing.DataSetupTime = 255;
	Timing.DataHoldTime = 3;
	Timing.BusTurnAroundDuration = 15;
	Timing.CLKDivision = 16;
	Timing.DataLatency = 17;
	Timing.AccessMode = FMC_ACCESS_MODE_A;
	/* ExtTiming */

	if (HAL_SRAM_Init(&hsram1, &Timing, NULL) != HAL_OK)
	{
		Error_Handler( );
	}

	/** Perform the NAND1 memory initialization sequence
	 */
	hnand1.Instance = FMC_NAND_DEVICE;
	/* hnand1.Init */
	hnand1.Init.NandBank = FMC_NAND_BANK3;
	hnand1.Init.Waitfeature = FMC_NAND_WAIT_FEATURE_ENABLE;
	hnand1.Init.MemoryDataWidth = FMC_NAND_MEM_BUS_WIDTH_8;
	hnand1.Init.EccComputation = FMC_NAND_ECC_ENABLE;
	hnand1.Init.ECCPageSize = FMC_NAND_ECC_PAGE_SIZE_8192BYTE;
	hnand1.Init.TCLRSetupTime = 15;
	hnand1.Init.TARSetupTime = 15;
	/* hnand1.Config */
	hnand1.Config.PageSize = 8192;
	hnand1.Config.SpareAreaSize = 448;
	hnand1.Config.BlockSize = 128;
	hnand1.Config.BlockNbr = 4096;
	hnand1.Config.PlaneNbr = 2;
	hnand1.Config.PlaneSize = 2048;
	hnand1.Config.ExtraCommandEnable = ENABLE;
	/* ComSpaceTiming */
	ComSpaceTiming.SetupTime = 19;
	ComSpaceTiming.WaitSetupTime = 19;
	ComSpaceTiming.HoldSetupTime = 6;
	ComSpaceTiming.HiZSetupTime = 3;
	/* AttSpaceTiming */
	AttSpaceTiming.SetupTime = 19;
	AttSpaceTiming.WaitSetupTime = 19;
	AttSpaceTiming.HoldSetupTime = 10;
	AttSpaceTiming.HiZSetupTime = 9;

	if (HAL_NAND_Init(&hnand1, &ComSpaceTiming, &AttSpaceTiming) != HAL_OK)
	{
		Error_Handler( );
	}

	/* USER CODE BEGIN FMC_Init 2 */

	/* USER CODE END FMC_Init 2 */
}

/**
 * @brief GPIO Initialization Function
 * @param None
 * @retval None
 */
static void MX_GPIO_Init(void)
{
	GPIO_InitTypeDef GPIO_InitStruct = {0};
	/* USER CODE BEGIN MX_GPIO_Init_1 */
	/* USER CODE END MX_GPIO_Init_1 */

	/* GPIO Ports Clock Enable */
	__HAL_RCC_GPIOE_CLK_ENABLE();
	__HAL_RCC_GPIOC_CLK_ENABLE();
	__HAL_RCC_GPIOF_CLK_ENABLE();
	__HAL_RCC_GPIOH_CLK_ENABLE();
	__HAL_RCC_GPIOA_CLK_ENABLE();
	__HAL_RCC_GPIOB_CLK_ENABLE();
	__HAL_RCC_GPIOG_CLK_ENABLE();
	__HAL_RCC_GPIOD_CLK_ENABLE();

	/*Configure GPIO pin Output Level */
	HAL_GPIO_WritePin(GPIOF, CS_1_Pin|START_1_Pin|RTC_RST_Pin, GPIO_PIN_RESET);

	/*Configure GPIO pin Output Level */
	HAL_GPIO_WritePin(RESET_1_GPIO_Port, RESET_1_Pin, GPIO_PIN_RESET);

	/*Configure GPIO pin Output Level */
	HAL_GPIO_WritePin(GPIOA, RS422_CTRL_Pin|RS485_CTRL1_Pin|RS485_CTRL2_Pin|Pwr_Ctrl1_Pin
			|Pwr_Ctrl2_Pin|START_3_Pin|RESET_3_Pin, GPIO_PIN_RESET);

	/*Configure GPIO pin Output Level */
	HAL_GPIO_WritePin(GPIOB, GPO_1_Pin|GPO_2_Pin|CS_2_Pin, GPIO_PIN_RESET);

	/*Configure GPIO pin Output Level */
	HAL_GPIO_WritePin(GPIOE, LED1_Pin|LED2_Pin|LED3_Pin|LED4_Pin
			|LED5_Pin, GPIO_PIN_RESET);

	/*Configure GPIO pin Output Level */
	HAL_GPIO_WritePin(GPIOB, GPIO_PIN_10, GPIO_PIN_SET);

	/*Configure GPIO pin Output Level */
	HAL_GPIO_WritePin(GPIOG, START_2_Pin|RESET_2_Pin, GPIO_PIN_RESET);

	/*Configure GPIO pin Output Level */
	HAL_GPIO_WritePin(CS_3_GPIO_Port, CS_3_Pin, GPIO_PIN_RESET);

	/*Configure GPIO pins : GPI_3_Pin GPI_4_Pin GPI_5_Pin */
	GPIO_InitStruct.Pin = GPI_3_Pin|GPI_4_Pin|GPI_5_Pin;
	GPIO_InitStruct.Mode = GPIO_MODE_INPUT;
	GPIO_InitStruct.Pull = GPIO_NOPULL;
	HAL_GPIO_Init(GPIOE, &GPIO_InitStruct);

	/*Configure GPIO pins : CS_1_Pin START_1_Pin */
	GPIO_InitStruct.Pin = CS_1_Pin|START_1_Pin;
	GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
	GPIO_InitStruct.Pull = GPIO_NOPULL;
	GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_VERY_HIGH;
	HAL_GPIO_Init(GPIOF, &GPIO_InitStruct);

	/*Configure GPIO pin : DRDY_1_Pin */
	GPIO_InitStruct.Pin = DRDY_1_Pin;
	GPIO_InitStruct.Mode = GPIO_MODE_IT_FALLING;
	GPIO_InitStruct.Pull = GPIO_NOPULL;
	HAL_GPIO_Init(DRDY_1_GPIO_Port, &GPIO_InitStruct);

	/*Configure GPIO pin : RESET_1_Pin */
	GPIO_InitStruct.Pin = RESET_1_Pin;
	GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
	GPIO_InitStruct.Pull = GPIO_NOPULL;
	GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_VERY_HIGH;
	HAL_GPIO_Init(RESET_1_GPIO_Port, &GPIO_InitStruct);

	/*Configure GPIO pins : RS422_CTRL_Pin RS485_CTRL1_Pin RS485_CTRL2_Pin Pwr_Ctrl1_Pin
                           Pwr_Ctrl2_Pin */
	GPIO_InitStruct.Pin = RS422_CTRL_Pin|RS485_CTRL1_Pin|RS485_CTRL2_Pin|Pwr_Ctrl1_Pin
			|Pwr_Ctrl2_Pin;
	GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
	GPIO_InitStruct.Pull = GPIO_NOPULL;
	GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
	HAL_GPIO_Init(GPIOA, &GPIO_InitStruct);

	/*Configure GPIO pins : GPO_1_Pin GPO_2_Pin */
	GPIO_InitStruct.Pin = GPO_1_Pin|GPO_2_Pin;
	GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
	GPIO_InitStruct.Pull = GPIO_NOPULL;
	GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
	HAL_GPIO_Init(GPIOB, &GPIO_InitStruct);

	/*Configure GPIO pin : RTC_RST_Pin */
	GPIO_InitStruct.Pin = RTC_RST_Pin;
	GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
	GPIO_InitStruct.Pull = GPIO_NOPULL;
	GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
	HAL_GPIO_Init(RTC_RST_GPIO_Port, &GPIO_InitStruct);

	/*Configure GPIO pins : LED1_Pin LED2_Pin LED3_Pin LED4_Pin
                           LED5_Pin */
	GPIO_InitStruct.Pin = LED1_Pin|LED2_Pin|LED3_Pin|LED4_Pin
			|LED5_Pin;
	GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
	GPIO_InitStruct.Pull = GPIO_NOPULL;
	GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
	HAL_GPIO_Init(GPIOE, &GPIO_InitStruct);

	/*Configure GPIO pins : PB10 CS_2_Pin */
	GPIO_InitStruct.Pin = GPIO_PIN_10|CS_2_Pin;
	GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
	GPIO_InitStruct.Pull = GPIO_NOPULL;
	GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_VERY_HIGH;
	HAL_GPIO_Init(GPIOB, &GPIO_InitStruct);

	/*Configure GPIO pins : START_2_Pin RESET_2_Pin */
	GPIO_InitStruct.Pin = START_2_Pin|RESET_2_Pin;
	GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
	GPIO_InitStruct.Pull = GPIO_NOPULL;
	GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_VERY_HIGH;
	HAL_GPIO_Init(GPIOG, &GPIO_InitStruct);

	/*Configure GPIO pin : DRDY_2_Pin */
	GPIO_InitStruct.Pin = DRDY_2_Pin;
	GPIO_InitStruct.Mode = GPIO_MODE_IT_FALLING;
	GPIO_InitStruct.Pull = GPIO_NOPULL;
	HAL_GPIO_Init(DRDY_2_GPIO_Port, &GPIO_InitStruct);

	/*Configure GPIO pins : START_3_Pin RESET_3_Pin */
	GPIO_InitStruct.Pin = START_3_Pin|RESET_3_Pin;
	GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
	GPIO_InitStruct.Pull = GPIO_NOPULL;
	GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_VERY_HIGH;
	HAL_GPIO_Init(GPIOA, &GPIO_InitStruct);

	/*Configure GPIO pin : DRDY_3_Pin */
	GPIO_InitStruct.Pin = DRDY_3_Pin;
	GPIO_InitStruct.Mode = GPIO_MODE_IT_FALLING;
	GPIO_InitStruct.Pull = GPIO_NOPULL;
	HAL_GPIO_Init(DRDY_3_GPIO_Port, &GPIO_InitStruct);

	/*Configure GPIO pins : GPI_1_Pin GPI_2_Pin */
	GPIO_InitStruct.Pin = GPI_1_Pin|GPI_2_Pin;
	GPIO_InitStruct.Mode = GPIO_MODE_INPUT;
	GPIO_InitStruct.Pull = GPIO_NOPULL;
	HAL_GPIO_Init(GPIOD, &GPIO_InitStruct);

	/*Configure GPIO pin : CS_3_Pin */
	GPIO_InitStruct.Pin = CS_3_Pin;
	GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
	GPIO_InitStruct.Pull = GPIO_NOPULL;
	GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_VERY_HIGH;
	HAL_GPIO_Init(CS_3_GPIO_Port, &GPIO_InitStruct);

	/* EXTI interrupt init*/
	HAL_NVIC_SetPriority(EXTI1_IRQn, 0, 0);
	HAL_NVIC_EnableIRQ(EXTI1_IRQn);

	HAL_NVIC_SetPriority(EXTI7_IRQn, 0, 0);
	HAL_NVIC_EnableIRQ(EXTI7_IRQn);

	HAL_NVIC_SetPriority(EXTI11_IRQn, 0, 0);
	HAL_NVIC_EnableIRQ(EXTI11_IRQn);

	/* USER CODE BEGIN MX_GPIO_Init_2 */
	/* USER CODE END MX_GPIO_Init_2 */
}

/* USER CODE BEGIN 4 */


void UART_InitAllHandlers(void) {
	// Start receiving data on all four UARTs
	HAL_UART_Receive_IT(&huart4, &uart1_rx_buffer[0], sizeof(uart1_rx_buffer));
	HAL_UART_Receive_IT(&huart8, &uart2_rx_buffer[0], sizeof(uart2_rx_buffer));
	HAL_UART_Receive_IT(&huart5, &uart3_rx_buffer[0], sizeof(uart3_rx_buffer));
	HAL_UART_Receive_IT(&huart10, &uart4_rx_buffer[0], sizeof(uart4_rx_buffer));
}

void HAL_UART_RxCpltCallback(UART_HandleTypeDef *huart) {

	if (huart->Instance == UART4) {
		HAL_UART_Receive_IT(&huart4, &uart1_rx_buffer[0], sizeof(uart1_rx_buffer));

		if(uart1_rx_buffer[0] == '#')//T
		{
			command_str[command_str_index++] = uart1_rx_buffer[0] ;
			command_rcv  =1 ;

		}
		else if(command_rcv==1)
		{
			command_str[command_str_index++] = uart1_rx_buffer[0] ;
			if(command_str_index > 15)
			{
				command_str_index = 0 ;
				command_rcv = 0 ;
			}
		}
		if(command_rcv == 1 && uart1_rx_buffer[0] == '$')
		{
			if(command_str[command_str_index-1] == '$')
			{
				ATP_test  = 1;
				command_str_index = 0 ;
				command_rcv = 0 ;
			}
			else if((command_str[1]=='2')&&(command_str[2]=='9'))
			{
				ATP_test  = 1;
				command_str_index = 0 ;
				command_rcv = 0 ;
			}
			else
			{
				command_str_index = 0 ;
				command_rcv = 0 ;
				memset(command_str,0,sizeof(command_str));
			}
		}
	}

	else if (huart->Instance == UART5) {

		// Handle data received from UART5
		HAL_UART_Receive_IT(&huart5, &uart3_rx_buffer[0], 1);

		if(uart3_rx_buffer[0] == '@' )//S
		{
			time_str_length=0;
			time_str[time_str_length++] = uart3_rx_buffer[0];
			time_rcv =1;
		}
		else if(time_rcv==1)
		{
			time_str[time_str_length++] = uart3_rx_buffer[0];
			if(time_str_length > 22)
			{
				time_str_length = 0 ;
				time_rcv = 0 ;
			}
		}
		if(uart3_rx_buffer[0] == '$' )//E
		{
			if(time_str_length == 21)
			{
				//time_str[time_str_length++] = uart3_rx_buffer;
				time_set = 1;
			}
			else
			{
				time_rcv = 0 ;
				time_str_length = 0 ;
				memset(time_str,0,sizeof(time_str));
			}
		}

		if(uart3_rx_buffer[0] == '#')//T
		{
			command_str[command_str_index++] = uart3_rx_buffer[0] ;
			command_rcv  =1 ;

		}
		else if(command_rcv==1)
		{
			command_str[command_str_index++] = uart3_rx_buffer[0] ;
			if(command_str_index > 15)
			{
				command_str_index = 0 ;
				command_rcv = 0 ;
			}
		}
		if(command_rcv == 1 && uart3_rx_buffer[0] == '$')
		{
			if(command_str_index < 4)
			{
				memcpy(nand_stop_read_CMD,command_str,command_str_index); // #*$
				memset(command_str,0,sizeof(command_str));
				command_str_index = 0 ;
				command_rcv = 0 ;
			}
			else if(command_str_index == 4)
			{
				ATP_test  = 1;
				command_str_index = 0 ;
				command_rcv = 0 ;
			}
			else if((command_str[1]=='2')&&(command_str[2]=='6'))
			{
				ATP_test  = 1;
				command_str_index = 0 ;
				command_rcv = 0 ;
			}
			else
			{
				command_str_index = 0 ;
				command_rcv = 0 ;
				memset(command_str,0,sizeof(command_str));
			}
		}

	}
	else if (huart->Instance == UART8) {
		// Handle data received from UART2
		HAL_UART_Receive_IT(&huart8, &uart2_rx_buffer[0], sizeof(uart2_rx_buffer));

		if(uart2_rx_buffer[0] == '#')//T
		{
			command_str[command_str_index++] = uart2_rx_buffer[0] ;
			command_rcv  =1 ;

		}
		else if(command_rcv==1)
		{
			command_str[command_str_index++] = uart2_rx_buffer[0] ;
			if(command_str_index > 15)
			{
				command_str_index = 0 ;
				command_rcv = 0 ;
			}
		}
		if(command_rcv == 1 && uart2_rx_buffer[0] == '$')
		{

			if((command_str[1]=='2')&&(command_str[2]=='7'))
			{
				ATP_test  = 1;
				command_str_index = 0 ;
				command_rcv = 0 ;
			}
			else
			{
				command_str_index = 0 ;
				command_rcv = 0 ;
				memset(command_str,0,sizeof(command_str));
			}
		}
	}
	else if (huart->Instance == USART10) {
		// Handle data received from UART4

		HAL_UART_Receive_IT(&huart10, &uart4_rx_buffer[0], sizeof(uart4_rx_buffer));

		if(uart4_rx_buffer[0] == '#')//T
		{
			command_str[command_str_index++] = uart4_rx_buffer[0] ;
			command_rcv  =1 ;

		}
		else if(command_rcv==1)
		{
			command_str[command_str_index++] = uart4_rx_buffer[0] ;
			if(command_str_index > 15)
			{
				command_str_index = 0 ;
				command_rcv = 0 ;
			}
		}
		if(command_rcv == 1 && uart4_rx_buffer[0] == '$')
		{
			if(command_str[command_str_index-1] == '$')
			{
				ATP_test  = 1;
				command_str_index = 0 ;
				command_rcv = 0 ;
			}
			else if((command_str[1]=='2')&&(command_str[2]=='8'))
			{
				ATP_test  = 1;
				command_str_index = 0 ;
				command_rcv = 0 ;
			}
			else
			{
				command_str_index = 0 ;
				command_rcv = 0 ;
				memset(command_str,0,sizeof(command_str));
			}
		}
	}

}



// User-defined callback to handle the data-ready event
void HAL_GPIO_EXTI_Falling_Callback(uint16_t GPIO_Pin)
{
	if (GPIO_Pin == DRDY_1_Pin)  // Check if the interrupt was caused by pin 0
	{

		//HAL_UART_Transmit_IT(&huart8, "DR_1\n\r", strlen("DR_1\n\r"));
		//HAL_Delay(1000);
		// Data ready logic
		// For example, read data from a sensor using SPI/I2C
	}
	else if (GPIO_Pin == DRDY_2_Pin)  // Check if the interrupt was caused by pin 0
	{
		//HAL_UART_Transmit_IT(&huart8, "DR_2\n\r", strlen("DR_1\n\r"));
		//HAL_Delay(1);
		// Data ready logic
		// For example, read data from a sensor using SPI/I2C
	}
	else if (GPIO_Pin == DRDY_3_Pin)  // Check if the interrupt was caused by pin 0
	{

		//HAL_UART_Transmit_IT(&huart8, "DR_3\n\r", strlen("DR_1\n\r"));
		//HAL_Delay(1);
		// Data ready logic
		// For example, read data from a sensor using SPI/I2C
	}
}
/* USER CODE END 4 */

/**
 * @brief  This function is executed in case of error occurrence.
 * @retval None
 */
void Error_Handler(void)
{
	/* USER CODE BEGIN Error_Handler_Debug */
	/* User can add his own implementation to report the HAL error return state */
	__disable_irq();
	while (1)
	{

	}
	/* USER CODE END Error_Handler_Debug */
}

#ifdef  USE_FULL_ASSERT
/**
 * @brief  Reports the name of the source file and the source line number
 *         where the assert_param error has occurred.
 * @param  file: pointer to the source file name
 * @param  line: assert_param error line source number
 * @retval None
 */
void assert_failed(uint8_t *file, uint32_t line)
{
	/* USER CODE BEGIN 6 */
	/* User can add his own implementation to report the file name and line number,
     ex: printf("Wrong parameters value: file %s on line %d\r\n", file, line) */
	/* USER CODE END 6 */
}
#endif /* USE_FULL_ASSERT */
