/*
 * SRAM_Operations.c
 *
 *  Created on: Nov 18, 2024
 *      Author: AEM
 */

/* Includes ------------------------------------------------------------------*/
#include "main.h"

/* Private includes ----------------------------------------------------------*/
#include <stdio.h>
#include <string.h>
#include "ATP_Mode.h"
#include "SRAM_Operations.h"


// SRAM and NAND addresses
//uint8_t data[8192] = {0};
uint32_t sram_location = 0x60000010;
uint32_t sram_base_location = 0x60000010;
uint32_t sramAddr_base_addr = 0x60000000;
uint32_t sramAddr_NandCyclic = 0x6000000C; // nand_cyclic_run
uint32_t Last_sramAddr =  0x6000000E; // last SRAM address store
uint32_t sec_addr = 0x60000000 | 0x7FFF9;
uint32_t min_addr = 0x60000000 | 0x7FFFa;
uint32_t hr_addr = 0x60000000 | 0x7FFFb;
uint32_t day_addr = 0x60000000 | 0x7FFFc;
uint32_t date_addr = 0x60000000 | 0x7FFFd;
uint32_t month_addr = 0x60000000 | 0x7FFFe;
uint32_t year_addr = 0x60000000 | 0x7FFFf;
uint32_t page_addr = 0x60000000;
uint32_t block_addr = 0x60000002;
uint32_t plane_addr = 0x60000004;
uint32_t size_counter_addr = 0x6000000C;
uint32_t unused_sramAddr =  0x60002FFF;
uint8_t unused = 0xAA;
// Buffers for SRAM operations
uint8_t SRAM_buffer[300] = {0};
uint8_t pDstBuffer = 0;
uint8_t pSrcBuffer = 0x80;
uint32_t BufferSize = 1;
uint8_t sram_buffer[25] = {0};
uint8_t dataToWrite[256];
uint16_t size_counter = 0;
uint8_t old_sec = 0;
uint8_t cur_sec = 0;

/*
 * structure variable
 */
_sSRAMOperation SRAMopt;

/* Function Prototypes -------------------------------------------------------*/
void time_setter();
uint8_t Init_NAND_addr();
void update_NAND_addr();
uint8_t SRAM_operation();
void get_NAND_data(uint8_t *sram_data);
uint8_t get_NAND_address(void);
void update_NAND_CyclicAddr();
uint8_t get_NAND_CyclicAddr(void);
void update_SRAM_Addr();
uint32_t get_SRAM_Addr(void);

/*
 * SRAM_operation
 * Reads time data from SRAM, updates temperature readings, and writes formatted
 * data back to SRAM. Checks for time changes and triggers NAND update if necessary.
 */
uint8_t SRAM_operation() {

	uint8_t time_data[8] = {0};
	// Read current second from SRAM
	HAL_SRAM_Read_8b(&hsram1, (uint32_t *)sec_addr, (uint8_t *)&time_data, 7);
	HAL_SRAM_Write_8b(&hsram1, (uint32_t *)unused_sramAddr, (uint8_t *)&unused, 1);
	// Check if the second has changed
	if (time_data[0] != old_sec) {
		HAL_GPIO_WritePin(GPIOE, LED4_Pin, 1); // Toggle LED for visual feedback
		HAL_Delay(1);
		HAL_GPIO_WritePin(GPIOE, LED4_Pin, 0);

		// Format temperature and time data into the buffer
		memset(SRAMopt.SRAM_buffer,0,sizeof(SRAMopt.SRAM_buffer));
		SRAMopt.Startbyte=SoF;
		SRAMopt.Idxcount=sprintf((char *)SRAMopt.SRAM_buffer,
				"{\"TS1\": \"%.2f#%X\", \"TS2\": \"%.2f#%X\", \"TS3\": \"%.2f#%X\", \"TS4\": \"%.2f#%X\", \"TS5\": \"%.2f#%X\", "
				"\"TS6\": \"%.2f#%X\", \"TS7\": \"%.2f#%X\", \"TS8\": \"%.2f#%X\", \"TS9\": \"%.2f#%X\", \"TS10\": \"%.2f#%X\", "
				"\"TS11\": \"%.2f#%X\", \"TS12\": \"%.2f#%X\", \"TIME\": \"%x:%x:%x\",\"DATE\": \"%x-%x-20%x\"} \n\r",
				temp_sensor[adc_1][channel_1],sensor_hex[adc_1][channel_1] ,temp_sensor[adc_1][channel_2],sensor_hex[adc_1][channel_2], temp_sensor[adc_1][channel_3],sensor_hex[adc_1][channel_3], temp_sensor[adc_1][channel_4],sensor_hex[adc_1][channel_4],
				temp_sensor[adc_2][channel_1],sensor_hex[adc_2][channel_1], temp_sensor[adc_2][channel_2],sensor_hex[adc_2][channel_2],temp_sensor[adc_2][channel_3],sensor_hex[adc_2][channel_3], temp_sensor[adc_2][channel_4],sensor_hex[adc_2][channel_4],
				temp_sensor[adc_3][channel_1],sensor_hex[adc_3][channel_1], temp_sensor[adc_3][channel_2],sensor_hex[adc_3][channel_2], temp_sensor[adc_3][channel_3],sensor_hex[adc_3][channel_3], temp_sensor[adc_3][channel_4],sensor_hex[adc_3][channel_4],
				time_data[Hour], time_data[Minute], time_data[Second], time_data[Date], time_data[Month], time_data[Year]);
		SRAMopt.SRAM_buffer[(SRAMopt.Idxcount)-3]=EoF;

		// Write the buffer to SRAM
		HAL_SRAM_Write_8b(&hsram1, (uint32_t *)sram_location, (uint8_t *)&SRAMopt,((SRAMopt.Idxcount)+1));


		// Update SRAM location and size counter
		sram_location += ((SRAMopt.Idxcount)+1);
		size_counter  += ((SRAMopt.Idxcount)+1);

#if 1
		HAL_GPIO_WritePin(GPIOA, RS485_CTRL1_Pin, GPIO_PIN_SET);
		HAL_GPIO_WritePin(GPIOA, RS485_CTRL2_Pin, GPIO_PIN_SET);
		HAL_GPIO_WritePin(GPIOA, RS422_CTRL_Pin,  GPIO_PIN_SET);
		HAL_Delay(2);
		HAL_UART_Transmit_IT(&huart8, (uint8_t *)&SRAMopt, (SRAMopt.Idxcount)+1);
		HAL_UART_Transmit_IT(&huart10,(uint8_t *)&SRAMopt, (SRAMopt.Idxcount)+1);
		HAL_UART_Transmit_IT(&huart4, (uint8_t *)&SRAMopt, (SRAMopt.Idxcount)+1);
		HAL_UART_Transmit_IT(&huart5, (uint8_t *)&SRAMopt, (SRAMopt.Idxcount)+1);
		HAL_Delay(100);
		HAL_GPIO_WritePin(GPIOA, RS422_CTRL_Pin,  GPIO_PIN_RESET);
		HAL_GPIO_WritePin(GPIOA, RS485_CTRL1_Pin, GPIO_PIN_RESET);
		HAL_GPIO_WritePin(GPIOA, RS485_CTRL2_Pin, GPIO_PIN_RESET);
#endif

//		HAL_Delay(200); // remove after testing

		// Check if the buffer exceeds the NAND page size
		if (size_counter + ((SRAMopt.Idxcount)+1) > NAND_PAGE_SIZE) {
			sram_location = sram_base_location;
			memset(SRAMopt.SRAM_buffer,0,sizeof(SRAMopt.SRAM_buffer));
			SRAMopt.Idxcount=0;
			NAND_operation();
		}

		update_SRAM_Addr();   /*********** update the sram last address at location 0x6000000e ************/

		// Update old_sec to current second
		old_sec = time_data[0];
	}

	// If time setting flag is set, call time_setter
	if (time_set) {
		time_set = 0;
		time_setter();
	}
	return 0;
}

/*
 * get_NAND_data
 * Reads a full page of data from SRAM into sram_data buffer and clears SRAM.
 */
void get_NAND_data(uint8_t *sram_data) {
	uint8_t CLEAN_DATA[8192] = {0};
	// Read NAND page from SRAM
	HAL_SRAM_Read_8b(&hsram1, (uint32_t *)sram_base_location, sram_data, NAND_PAGE_SIZE);
	// Clear the SRAM location
	HAL_SRAM_Write_8b(&hsram1, (uint32_t *)sram_base_location, CLEAN_DATA, NAND_PAGE_SIZE);
}

/*
 * get_NAND_address
 * Reads the current NAND address from SRAM.
 */
uint8_t get_NAND_address(void) {
	HAL_SRAM_Read_8b(&hsram1, (uint32_t *)sramAddr_base_addr + 2, (uint8_t *)&nand_address, sizeof(nand_address));
	if(nand_address.Page >= 128)
		nand_address.Page=0;
	if(nand_address.Block >= 4096)
		nand_address.Block=0;
	if(nand_address.Plane > 2)
		nand_address.Plane=0;

	return 0;
}

/*
 * update_NAND_addr
 * Writes the current NAND address to SRAM.
 */
void update_NAND_addr() {
	HAL_SRAM_Write_8b(&hsram1, (uint32_t *)sramAddr_base_addr + 2, (uint8_t *)&nand_address, sizeof(nand_address));
}

/*
 * update_NAND_CyclicAddr
 * Writes the current nand_cyclic_run to SRAM.
 */
void update_NAND_CyclicAddr() {
	HAL_SRAM_Write_8b(&hsram1, (uint32_t *)sramAddr_NandCyclic, (uint8_t *)&nand_cyclic_run, sizeof(nand_cyclic_run));
}

/*
 * get_NAND_CyclicAddr
 * Reads the last nand_cyclic_run from SRAM.
 */
uint8_t get_NAND_CyclicAddr(void) {
	HAL_SRAM_Read_8b(&hsram1, (uint32_t *)sramAddr_NandCyclic, (uint8_t *)&nand_cyclic_run, sizeof(nand_cyclic_run));
	return nand_cyclic_run;
}

/*
 * update_SRAM_Addr
 * Writes the current size_counter to SRAM.
 */
void update_SRAM_Addr() {
	HAL_SRAM_Write_8b(&hsram1, (uint32_t *)Last_sramAddr, (uint8_t *)&size_counter, sizeof(size_counter));
	if (nand_update==1)
	{
		nand_update=0;
		size_counter=0;
	}
}

/*
 * get_SRAM_Addr
 * Reads the last size_counter from SRAM.
 */
uint32_t get_SRAM_Addr(void) {
	//	_sSRAM_LastAddr SRAM_LastAddr;
	uint32_t last_update_location = 0;
	//	memset(&SRAM_LastAddr,0,sizeof(SRAM_LastAddr));
	HAL_SRAM_Read_8b(&hsram1, (uint32_t *)Last_sramAddr, (uint8_t *)&size_counter, sizeof(size_counter));

	if(size_counter==0){
		return sram_base_location;
	}
	else if((size_counter+251) > NAND_PAGE_SIZE){
		size_counter=0;
		update_SRAM_Addr();
		return sram_base_location;
	}
	else{
		last_update_location = (sram_base_location + size_counter);
	}
	//	SRAM_LastAddr.sram_count= size_counter;
	//	SRAM_LastAddr.last_update_location = (sram_base_location + size_counter);

	return last_update_location;
}

uint8_t Init_NAND_addr()
{
	nand_address.Page=0;
	nand_address.Block=0;
	nand_address.Plane=0;
	update_NAND_addr();
	return 0;
}

/*
 * time_setter
 * Sets the time in SRAM using the format "SDD-MM-YYYY HH:MM:SS E".
 */
void time_setter() {
	uint8_t day, month, year, hours, minutes, seconds;
	//	0 1 2 3 4 5 6 7 8 9 10 11 12 13 14 15 16 17 18 19 20
	//	@ D D - M M - Y Y Y  Y     H  H  :  M  M  :  S  S  $
	// Parse the time string if it follows the expected format
	if (time_str[0] == '@' && time_str[20] == '$' &&
			time_str[5] == '-' && time_str[8] == '-' &&
			time_str[11] == ' ' && time_str[14] == ':' && time_str[17] == ':') {

		// Extract date and time components from time_str
		month = (time_str[6] - '0') * 16 + (time_str[7] - '0');
		year = (time_str[3] - '0') * 16 + (time_str[4] - '0');
		day = (time_str[9] - '0') * 16 + (time_str[10] - '0');
		minutes = (time_str[15] - '0') * 16 + (time_str[16] - '0');
		hours = (time_str[12] - '0') * 16 + (time_str[13] - '0');
		seconds = (time_str[18] - '0') * 16 + (time_str[19] - '0');

		// Write time components to SRAM
		sramAddr = 0x60000000 | 0x7FFF8;
		pSrcBuffer = 0x80;
		HAL_SRAM_Write_8b(&hsram1, (uint32_t *)sramAddr, &pSrcBuffer, BufferSize);

		sramAddr = 0x60000000 | 0x7FFF9;
		pSrcBuffer = seconds;
		HAL_SRAM_Write_8b(&hsram1, (uint32_t *)sramAddr, &pSrcBuffer, BufferSize);

		sramAddr = 0x60000000 | 0x7FFFa;
		pSrcBuffer = minutes;
		HAL_SRAM_Write_8b(&hsram1, (uint32_t *)sramAddr, &pSrcBuffer, BufferSize);

		sramAddr = 0x60000000 | 0x7FFFb;
		pSrcBuffer = hours;
		HAL_SRAM_Write_8b(&hsram1, (uint32_t *)sramAddr, &pSrcBuffer, BufferSize);

		sramAddr = 0x60000000 | 0x7FFFc;
		pSrcBuffer = 0x02;
		HAL_SRAM_Write_8b(&hsram1, (uint32_t *)sramAddr, &pSrcBuffer, BufferSize);

		sramAddr = 0x60000000 | 0x7FFFd;
		pSrcBuffer = day;
		HAL_SRAM_Write_8b(&hsram1, (uint32_t *)sramAddr, &pSrcBuffer, BufferSize);

		sramAddr = 0x60000000 | 0x7FFFe;
		pSrcBuffer = month;
		HAL_SRAM_Write_8b(&hsram1, (uint32_t *)sramAddr, &pSrcBuffer, BufferSize);

		sramAddr = 0x60000000 | 0x7FFFf;
		pSrcBuffer = year;
		HAL_SRAM_Write_8b(&hsram1, (uint32_t *)sramAddr, &pSrcBuffer, BufferSize);

		sramAddr = 0x60000000 | 0x7FFF8;
		pSrcBuffer = 0x00;
		HAL_SRAM_Write_8b(&hsram1, (uint32_t *)sramAddr, &pSrcBuffer, BufferSize);
	}
}


