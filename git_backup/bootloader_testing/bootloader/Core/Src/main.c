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
#include "stm32h5xx_hal_flash.h"
#include "xmodem.h"
/* USER CODE END Includes */

/* Private typedef -----------------------------------------------------------*/
/* USER CODE BEGIN PTD */

/* USER CODE END PTD */

/* Private define ------------------------------------------------------------*/
/* USER CODE BEGIN PD */

/* USER CODE END PD */

/* Private macro -------------------------------------------------------------*/
/* USER CODE BEGIN PM */
#define BOOTLOADER_GPIO_PIN GPIO_PIN_0  // GPIO pin for mode switching
#define BOOTLOADER_GPIO_PORT GPIOA      // GPIO port for mode switching
#define APPLICATION_START_ADDRESS 0x0800C000  // Address where the application is stored
#define TIMEOUT_MS 30000               // Timeout for bootloader (30 seconds)
#define UART_BUFFER_SIZE 256           // UART buffer size for file transfer
/* USER CODE END PM */

/* Private variables ---------------------------------------------------------*/

UART_HandleTypeDef huart5;

/* USER CODE BEGIN PV */
uint8_t uartBuffer[UART_BUFFER_SIZE];

/* USER CODE END PV */

/* Private function prototypes -----------------------------------------------*/
void SystemClock_Config(void);
static void MX_GPIO_Init(void);
static void MX_UART5_Init(void);
/* USER CODE BEGIN PFP */
void UART_InitAllHandlers(void) ;
/* USER CODE END PFP */

/* Private user code ---------------------------------------------------------*/
/* USER CODE BEGIN 0 */
bool IsBootloaderMode(void);
void JumpToApplication(void);
void BootloaderMain(void);
void EraseFlash(uint32_t startAddress, uint32_t length);
void WriteQuadWordToFlash(uint32_t flashAddress, uint32_t *data,
		uint32_t length);
void circular_buffer_write(CircularBuffer *cb, uint8_t value);
//#define BUFFER_SIZE 256; // Define the size of the buffer



uint8_t uart5_rx_buffer[1];

CircularBuffer uart_buffer;






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
	MX_UART5_Init();
	/* USER CODE BEGIN 2 */
	UART_InitAllHandlers();
	if(!(IsBootloaderMode())){
	uart_transmit_str((uint8_t*)"\n\r================================\n\r");
	uart_transmit_str((uint8_t*)"UART Bootloader\n\r");
	uart_transmit_str((uint8_t*)"Entering into bootloader mode, open Tera Term and send binary file in XMODEM mode\n\r");
	uart_transmit_str((uint8_t*)"================================\n\r\n\r");
	}
	else{
		flash_jump_to_app();
	}
	circular_buffer_init(&uart_buffer);



	/* USER CODE END 2 */

	/* Infinite loop */
	/* USER CODE BEGIN WHILE */
	while (1) {
#if 1
		if(!(IsBootloaderMode())){

			flash_erase(APPLICATION_START_ADDRESS);
			uart_transmit_str((uint8_t*)"Please send a new binary file with Xmodem protocol to update the firmware.\n\r");
			xmodem_receive(&huart5);
			HAL_Delay(wait_time);
			circular_buffer_init(&uart_buffer);
			uart_transmit_str((uint8_t*)"\n\rFailed... Please try again.\n\r");
		}
		else
//#else
			/*make sure your code is update into flash*/
			flash_jump_to_app();
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
	__HAL_RCC_GPIOH_CLK_ENABLE();
	__HAL_RCC_GPIOD_CLK_ENABLE();
	__HAL_RCC_GPIOB_CLK_ENABLE();

	/*Configure GPIO pin : GPIO_3_Pin */
	GPIO_InitStruct.Pin = GPIO_3_Pin;
	GPIO_InitStruct.Mode = GPIO_MODE_INPUT;
	GPIO_InitStruct.Pull = GPIO_NOPULL;
	HAL_GPIO_Init(GPIO_3_GPIO_Port, &GPIO_InitStruct);

	/*Configure GPIO pin : GPIO_2_Pin */
	GPIO_InitStruct.Pin = GPIO_2_Pin;
	GPIO_InitStruct.Mode = GPIO_MODE_INPUT;
	GPIO_InitStruct.Pull = GPIO_NOPULL;
	HAL_GPIO_Init(GPIO_2_GPIO_Port, &GPIO_InitStruct);

	/* USER CODE BEGIN MX_GPIO_Init_2 */
	/* USER CODE END MX_GPIO_Init_2 */
}

/* USER CODE BEGIN 4 */

void UART_InitAllHandlers(void) {
	// Start receiving data on all four UARTs
	HAL_UART_Receive_IT(&huart5, &uart5_rx_buffer[0], sizeof(uart5_rx_buffer));
}


// Function to jump to the user application
void JumpToApplication(void) {

	// Define a function pointer to the application's entry point
	void (*ApplicationEntryPoint)(void);

	// Get the application's stack pointer (first 4 bytes of the application address)
	uint32_t applicationStack = *((volatile uint32_t*) APPLICATION_START_ADDRESS);

	// Get the application's entry point (4 bytes after the stack pointer)
	ApplicationEntryPoint = (void (*)(void)) *((volatile uint32_t*) (APPLICATION_START_ADDRESS + 4));

	// Set the MSP to the application's stack pointer
	__set_MSP(applicationStack);

	// Jump to the application's entry point (start executing from there)
	ApplicationEntryPoint();

}

void BootloaderMain(void) {
	uint32_t flashStartAddress = APPLICATION_START_ADDRESS;  // Flash address for application
	uint8_t buffer[256];  // Declare buffer as a pointer
	//    uint32_t alignedBuffer[64];  // 32-bit aligned buffer (256 bytes = 64 * 4 bytes)
	//    buffer = (uint8_t *)alignedBuffer;  // Point buffer to alignedBuffer
	uint32_t remainingSize = 59392;

	// Ensure flashStartAddress is aligned to 16-byte boundary
	if (flashStartAddress % 16 != 0) {
		flashStartAddress = (flashStartAddress + 15) & ~15; // Adjust to next 16-byte boundary
	}

	if (flashStartAddress < 0x08000000 || flashStartAddress >= 0x081FFFFF) {
		return;  // Exit if the address is outside the valid flash range
	}

	// Notify user via UART
	const char *readyMessage = "BOOTLOADER READY: SEND FILE SIZE\n";
	HAL_UART_Transmit(&huart5, (uint8_t*) readyMessage, strlen(readyMessage),
			HAL_MAX_DELAY);

	// Receive the file size (4 bytes)
	// HAL_UART_Receive(&huart5, (uint8_t *)&remainingSize, sizeof(remainingSize), HAL_MAX_DELAY);

	// Erase the flash before writing
	EraseFlash(flashStartAddress, remainingSize);

	// Notify user to send the file
	const char *sendMessage = "SEND FILE DATA\n";
	HAL_UART_Transmit(&huart5, (uint8_t*) sendMessage, strlen(sendMessage),
			HAL_MAX_DELAY);

	// Receive and program the file in chunks
	while (remainingSize > 0) {
		// uint32_t chunkSize = (remainingSize > sizeof(alignedBuffer)) ? sizeof(alignedBuffer) : remainingSize;
		uint32_t chunkSize =
				(remainingSize > sizeof(buffer)) ?
						sizeof(buffer) : remainingSize;

		if (chunkSize > sizeof(buffer)) {
			// Handle the overflow, or adjust chunkSize to fit within buffer
		}

		// Adjust chunk size to be a multiple of 16
		if (chunkSize % 16 != 0) {
			chunkSize -= (chunkSize % 16); // Adjust chunk size to the nearest multiple of 16
		}

		// Receive a chunk of data
		HAL_StatusTypeDef status = HAL_UART_Receive(&huart5, buffer, chunkSize,
				50000);

		if (status != HAL_OK) {
			// Handle the timeout or error
			return;
		}

		// Write the chunk to flash
		WriteQuadWordToFlash(flashStartAddress, (uint32_t*) buffer, chunkSize);

		flashStartAddress += chunkSize;
		remainingSize -= chunkSize;
	}

	// Notify user of success
	const char *successMessage = "FILE TRANSFER COMPLETE. RESETTING...\n";
	HAL_UART_Transmit(&huart5, (uint8_t*) successMessage,
			strlen(successMessage), HAL_MAX_DELAY);

	HAL_NVIC_SystemReset();  // Reset to run the new application
}



void WriteQuadWordToFlash(uint32_t flashAddress, uint32_t *data,
		uint32_t length) {
	HAL_FLASH_Unlock();  // Unlock the flash for programming

	// Write data in 128-bit (16-byte) blocks
	for (uint32_t i = 0; i < length; i += 16) {
		if (HAL_FLASH_Program(FLASH_TYPEPROGRAM_QUADWORD, flashAddress + i,
				(uint32_t) (data + i / 4)) != HAL_OK) {
			Error_Handler();  // Handle programming error
		}
	}

	HAL_FLASH_Lock();  // Lock the flash after programming
}

void EraseFlash(uint32_t startAddress, uint32_t length) {
	FLASH_EraseInitTypeDef eraseInitStruct = { 0 };
	uint32_t pageError = 0;

	// Unlock the flash for erasing
	HAL_FLASH_Unlock();

	// Configure the erase parameters
	eraseInitStruct.TypeErase = FLASH_TYPEERASE_SECTORS; // Use sectors for STM32H5
	eraseInitStruct.Sector = startAddress;
	eraseInitStruct.NbSectors = (length + FLASH_SECTOR_SIZE - 1)
					/ FLASH_SECTOR_SIZE;

	// Perform the erase operation
	if (HAL_FLASHEx_Erase(&eraseInitStruct, &pageError) != HAL_OK) {
		Error_Handler();  // Handle erase error
	}

	// Lock the flash after erasing
	HAL_FLASH_Lock();
}

// Check if bootloader mode should run (e.g., check GPIO or specific condition)
bool IsBootloaderMode(void) {

	if ((HAL_GPIO_ReadPin(GPIOD, GPIO_PIN_3) == 1)
			&& (HAL_GPIO_ReadPin(GPIOE, GPIO_PIN_4) == 1)) {
		return GPIO_PIN_RESET; // Enter bootloader if button is pressed
	} else
		return GPIO_PIN_SET;

}


void HAL_UART_RxCpltCallback(UART_HandleTypeDef *huart) {

	if (huart->Instance == UART5) {
		// Handle data received from UART5
		HAL_UART_Receive_IT(&huart5, &uart5_rx_buffer[0], 1);

		circular_buffer_write(&uart_buffer, uart5_rx_buffer[0]);
	}

}

void circular_buffer_init(CircularBuffer *cb) {
	cb->head = 0;
	cb->tail = 0;
	cb->full = false;
}



void circular_buffer_write(CircularBuffer *cb, uint8_t value) {
	cb->data[cb->head] = value;

	// Advance the head pointer
	cb->head = (cb->head + 1) % BUFFER_SIZE;

	if (cb->full) {
		// If the buffer is full, advance the tail pointer
		cb->tail = (cb->tail + 1) % BUFFER_SIZE;
	}

	// Check if the buffer is full
	cb->full = (cb->head == cb->tail);
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
	while (1) {
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
