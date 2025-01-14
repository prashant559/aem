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
uint32_t Nand_Last_addr = 0x60000004;
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
uint8_t Die_count = 0;
//uint16_t Startbyte = ((uint16_t)SoF[0] << 8) | SoF[1];
//uint16_t Endbyte   = ((uint16_t)EoF[0] << 8) | EoF[1];
float NandBuf[]={0};

/*
 * structure variable
 */
_sRTCOperation RTCopt;
_sTSUFrame TSUframe;
_sMCSFrame MCSFrame;
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
	if (time_data[0] != old_sec)
	{
		HAL_GPIO_WritePin(GPIOE, LED4_Pin, 1); // Toggle LED for visual feedback
		HAL_Delay(1);
		HAL_GPIO_WritePin(GPIOE, LED4_Pin, 0);


		/*************************************************************************************************************
		 **********Conversion Factor at TSU****************
		 *************************************************************************************************************/
		memset(&TSUframe,0,sizeof(TSUframe));
		TSUframe.StartbyteI  =SoF_I;
		TSUframe.StartbyteII =SoF_II;
		TSUframe.TSU_Buf[0]  = (int16_t)(temp_sensor[adc_1][channel_1]*100);
		TSUframe.TSU_Buf[1]  = (int16_t)(temp_sensor[adc_1][channel_2]*100);
		TSUframe.TSU_Buf[2]  = (int16_t)(temp_sensor[adc_1][channel_3]*100);
		TSUframe.TSU_Buf[3]  = (int16_t)(temp_sensor[adc_1][channel_4]*100);
		TSUframe.TSU_Buf[4]  = (int16_t)(temp_sensor[adc_2][channel_1]*100);
		TSUframe.TSU_Buf[5]  = (int16_t)(temp_sensor[adc_2][channel_2]*100);
		TSUframe.TSU_Buf[6]  = (int16_t)(temp_sensor[adc_2][channel_3]*100);
		TSUframe.TSU_Buf[7]  = (int16_t)(temp_sensor[adc_2][channel_4]*100);
		TSUframe.TSU_Buf[8]  = (int16_t)(temp_sensor[adc_3][channel_1]*100);
		TSUframe.TSU_Buf[9]  = (int16_t)(temp_sensor[adc_3][channel_2]*100);
		TSUframe.TSU_Buf[10] = (int16_t)(temp_sensor[adc_3][channel_3]*100);
		TSUframe.TSU_Buf[11] = (int16_t)(temp_sensor[adc_3][channel_4]*100);
		TSUframe.Day		 = ((time_data[Date] & 0xF0) >> 4) * 10 + (time_data[Date] & 0x0F);
		TSUframe.Month 	     = ((time_data[Month] & 0xF0) >> 4) * 10 + (time_data[Month] & 0x0F);
		TSUframe.YearI		 = 20;
		TSUframe.YearII	 	 = ((time_data[Year] & 0xF0) >> 4) * 10 + (time_data[Year] & 0x0F);
		TSUframe.Hours       = ((time_data[Hour] & 0xF0) >> 4) * 10 + (time_data[Hour] & 0x0F);
		TSUframe.Minutes 	 = ((time_data[Minute] & 0xF0) >> 4) * 10 + (time_data[Minute] & 0x0F);
		TSUframe.Seconds 	 = ((time_data[Second] & 0xF0) >> 4) * 10 + (time_data[Second] & 0x0F);
		TSUframe.EndbyteI    = EoF_I;
		TSUframe.EndbyteII   = EoF_II;


		/*************************************************************************************************************
		 **********UART Transmission for GUI****************
		 *************************************************************************************************************/
#if 0
		HAL_GPIO_WritePin(GPIOA, RS485_CTRL1_Pin, GPIO_PIN_SET);
		HAL_GPIO_WritePin(GPIOA, RS485_CTRL2_Pin, GPIO_PIN_SET);
		HAL_GPIO_WritePin(GPIOA, RS422_CTRL_Pin,  GPIO_PIN_SET);
		HAL_Delay(2);
		HAL_UART_Transmit_IT(&huart8, (uint8_t *)&TSUframe, sizeof(TSUframe));
		HAL_UART_Transmit_IT(&huart10,(uint8_t *)&TSUframe, sizeof(TSUframe));
		HAL_UART_Transmit_IT(&huart4, (uint8_t *)&TSUframe, sizeof(TSUframe));
		HAL_UART_Transmit_IT(&huart5, (uint8_t *)&TSUframe, sizeof(TSUframe));
		HAL_Delay(100);
		HAL_GPIO_WritePin(GPIOA, RS422_CTRL_Pin,  GPIO_PIN_RESET);
		HAL_GPIO_WritePin(GPIOA, RS485_CTRL1_Pin, GPIO_PIN_RESET);
		HAL_GPIO_WritePin(GPIOA, RS485_CTRL2_Pin, GPIO_PIN_RESET);
#endif

		/***************for testing purpose**************************************************************************************/
		char uart_tx_buffer[256];
#if 0
		// Serialize the structure into the buffer
		snprintf(uart_tx_buffer, sizeof(uart_tx_buffer),
				"Start: %02X %02X, TSU_Buf: %d %d %d %d %d %d %d %d %d %d %d %d, "
				"Date: %02d/%02d/%02d%02d, Time: %02d:%02d:%02d, End: %02X %02X\n\r",
				TSUframe.StartbyteI, TSUframe.StartbyteII,
				TSUframe.TSU_Buf[0], TSUframe.TSU_Buf[1], TSUframe.TSU_Buf[2], TSUframe.TSU_Buf[3],
				TSUframe.TSU_Buf[4], TSUframe.TSU_Buf[5], TSUframe.TSU_Buf[6], TSUframe.TSU_Buf[7],
				TSUframe.TSU_Buf[8], TSUframe.TSU_Buf[9], TSUframe.TSU_Buf[10], TSUframe.TSU_Buf[11],
				TSUframe.Day, TSUframe.Month, TSUframe.YearI, TSUframe.YearII,
				TSUframe.Hours, TSUframe.Minutes, TSUframe.Seconds,
				TSUframe.EndbyteI, TSUframe.EndbyteII);

		// Transmit the serialized string
		HAL_UART_Transmit(&huart5, (uint8_t *)uart_tx_buffer, strlen(uart_tx_buffer),HAL_MAX_DELAY);
#endif
		/*************************************************************************************************************
		 **********Conversion Factor at MCS****************
		 *************************************************************************************************************/
		memset(&MCSFrame,0,sizeof(MCSFrame));
		MCSFrame.StartbyteI  =SoF_I;
		MCSFrame.StartbyteII =SoF_II;
		MCSFrame.MCS_Buf[0]  =(TSUframe.TSU_Buf[0])/100.0;
		MCSFrame.MCS_Buf[1]  =(TSUframe.TSU_Buf[1])/100.0;
		MCSFrame.MCS_Buf[2]  =(TSUframe.TSU_Buf[2])/100.0;
		MCSFrame.MCS_Buf[3]  =(TSUframe.TSU_Buf[3])/100.0;
		MCSFrame.MCS_Buf[4]  =(TSUframe.TSU_Buf[4])/100.0;
		MCSFrame.MCS_Buf[5]  =(TSUframe.TSU_Buf[5])/100.0;
		MCSFrame.MCS_Buf[6]  =(TSUframe.TSU_Buf[6])/100.0;
		MCSFrame.MCS_Buf[7]  =(TSUframe.TSU_Buf[7])/100.0;
		MCSFrame.MCS_Buf[8]  =(TSUframe.TSU_Buf[8])/100.0;
		MCSFrame.MCS_Buf[9]  =(TSUframe.TSU_Buf[9])/100.0;
		MCSFrame.MCS_Buf[10] =(TSUframe.TSU_Buf[10])/100.0;
		MCSFrame.MCS_Buf[11] =(TSUframe.TSU_Buf[11])/100.0;
		MCSFrame.Day         =TSUframe.Day;
		MCSFrame.Month 		 =TSUframe.Month;
		MCSFrame.YearI 		 =TSUframe.YearI;
		MCSFrame.YearII 	 =TSUframe.YearII;
		MCSFrame.Hours       =TSUframe.Hours;
		MCSFrame.Minutes     =TSUframe.Minutes;
		MCSFrame.Seconds     =TSUframe.Seconds;
		MCSFrame.EndbyteI    = EoF_I;
		MCSFrame.EndbyteII   = EoF_II;

		/***************for testing purpose**************************************************************************************/
#if 1
		memset(&uart_tx_buffer,0,sizeof(uart_tx_buffer));
		// Serialize the structure into the buffer
		snprintf(uart_tx_buffer, sizeof(uart_tx_buffer),
				"Start: %02X %02X, MCS_Buf: %2f %2f %2f %2f %2f %2f %2f %2f %2f %2f %2f %2f, "
				"Date: %02d/%02d/%02d%02d, Time: %02d:%02d:%02d, End: %02X %02X\n\r",
				MCSFrame.StartbyteI, MCSFrame.StartbyteII,
				MCSFrame.MCS_Buf[0], MCSFrame.MCS_Buf[1], MCSFrame.MCS_Buf[2], MCSFrame.MCS_Buf[3],
				MCSFrame.MCS_Buf[4], MCSFrame.MCS_Buf[5], MCSFrame.MCS_Buf[6], MCSFrame.MCS_Buf[7],
				MCSFrame.MCS_Buf[8], MCSFrame.MCS_Buf[9], MCSFrame.MCS_Buf[10], MCSFrame.MCS_Buf[11],
				MCSFrame.Day, MCSFrame.Month, MCSFrame.YearI, MCSFrame.YearII,
				MCSFrame.Hours, MCSFrame.Minutes, MCSFrame.Seconds,
				MCSFrame.EndbyteI, MCSFrame.EndbyteII);

		HAL_UART_Transmit(&huart5, (uint8_t *)uart_tx_buffer, strlen(uart_tx_buffer),HAL_MAX_DELAY);
		//HAL_Delay(100);
#endif
		/*****************************Write the Actual Values into SRAM******************************************************/
		HAL_SRAM_Write_8b(&hsram1, (uint32_t *)sram_location, (uint8_t *)&MCSFrame, sizeof(MCSFrame));
		HAL_Delay(50);

		// Update SRAM location and size counter
		sram_location += sizeof(MCSFrame);
		size_counter  += sizeof(MCSFrame);
		/********************maximum it will store up to 138 frames to make 8K of data******************************/
		// Check if the buffer exceeds the NAND page size
		if (size_counter + (sizeof(MCSFrame)) > NAND_PAGE_SIZE) {
			sram_location = sram_base_location;
			memset(&MCSFrame,0,sizeof(MCSFrame));
			memset(&TSUframe,0,sizeof(TSUframe));
			size_counter = 0;
			NAND_operation();
		}
		update_SRAM_Addr();   /* update the sram last address at location 0x6000000C */
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
	//HAL_SRAM_Read_8b(&hsram1, (uint32_t *)sram_base_location, sram_data, NAND_PAGE_SIZE);
	HAL_SRAM_Read_8b(&hsram1, (uint32_t *)sram_base_location, sram_data, NAND_PAGE_SIZE);
	// Clear the SRAM location
	HAL_SRAM_Write_8b(&hsram1, (uint32_t *)sram_base_location, CLEAN_DATA, NAND_PAGE_SIZE);
}

/*
 * get_NAND_address
 * Reads the current NAND address from SRAM.
 */
uint8_t get_NAND_address(void) {
	HAL_SRAM_Read_8b(&hsram1, (uint32_t *)Nand_Last_addr, (uint8_t *)&nand_address, sizeof(nand_address));
	if(nand_address.Page >= MAX_PAGE)
		nand_address.Page=0;

	if(nand_address.Block >= MAX_BLOCK){
		nand_address.Block=0;
		Die_count=1; //need to store in SRAM (opt to store location 0x6000000B)
	}
	if(Die_count == 1)
	{
		NAND_Die = 2;
		if(nand_address.Block >= 4095)
		{
			Die_count=0;
		}
	}
	else
	{
		NAND_Die = 1;
	}
	if(nand_address.Plane > 2)
		nand_address.Plane=0;

	return 0;
}

/*
 * update_NAND_addr
 * Writes the current NAND address to SRAM.
 */
void update_NAND_addr() {
	HAL_SRAM_Write_8b(&hsram1, (uint32_t *)Nand_Last_addr, (uint8_t *)&nand_address, sizeof(nand_address));
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
	else if((size_counter+ sizeof(MCSFrame)) > NAND_PAGE_SIZE){
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
 * this function will be used for convert 0x2025 -> 2025 in dec val
 */
#if 0
uint16_t Hx_conversion(uint16_t val) {
   uint8_t  high_val=0;
   uint8_t  low_val=0;

	uint16_t HiIdx_1 = (val >> 12) & 0xF;
	uint16_t HiIdx_2 = (val >> 8) & 0xF;
	uint16_t LoIdx_1 = (val >> 4) & 0xF;
	uint16_t LoIdx_2 = val & 0xF;

	// Convert BCD digits to decimal
   high_val= (HiIdx_1 * 10) + HiIdx_2;
   low_val= (LoIdx_1 * 10) + LoIdx_2;

}
#endif

uint8_t Decimal_to_BCD(uint8_t decimal_value) {
	return ((decimal_value / 10) << 4) | (decimal_value % 10);
}

/*
 * time_setter
 * Sets the time in SRAM using the format "SDD-MM-YYYY HH:MM:SS E".
 */
void time_setter() {
	//	uint8_t day, month, year, hours, minutes, seconds;
	//	0 1 2 3 4 5 6 7 8 9 10 11 12 13 14 15 16 17 18 19 20
	//	S D D - M M - Y Y Y  Y     H  H  :  M  M  :  S  S  E
	// Parse the time string if it follows the expected format
	//	if (time_str[0] == 'S' && time_str[20] == 'E' &&
	//			time_str[5] == '-' && time_str[8] == '-' &&
	//			time_str[11] == ' ' && time_str[14] == ':' && time_str[17] == ':') {
	//
	//		// Extract date and time components from time_str
	//		month = (time_str[6] - '0') * 16 + (time_str[7] - '0');
	//		year = (time_str[3] - '0') * 16 + (time_str[4] - '0');
	//		day = (time_str[9] - '0') * 16 + (time_str[10] - '0');
	//		minutes = (time_str[15] - '0') * 16 + (time_str[16] - '0');
	//		hours = (time_str[12] - '0') * 16 + (time_str[13] - '0');
	//		seconds = (time_str[18] - '0') * 16 + (time_str[19] - '0');
	//**int dec = ((hex & 0xF0) >> 4) * 10 + (hex & 0x0F);
	/******************************************************************************************************
	 ********* make frame for external synchronization for RTC **********
	 *******************************************************************************************************/

	RTCopt.StartbyteI 	= time_str[0];
	RTCopt.StartbyteII  = time_str[1];
	RTCopt.Day			= Decimal_to_BCD(time_str[2]);
	RTCopt.Month		= Decimal_to_BCD(time_str[3]);
	RTCopt.YearI		= time_str[4];
	RTCopt.YearII	    = Decimal_to_BCD(time_str[5]);
	RTCopt.Hours		= Decimal_to_BCD(time_str[6]);
	RTCopt.Minutes		= Decimal_to_BCD(time_str[7]);
	RTCopt.Seconds		= Decimal_to_BCD(time_str[8]);
	RTCopt.EndbyteI		= time_str[9];
	RTCopt.EndbyteII	=time_str[10];


	// Write time components to SRAM
	sramAddr = 0x60000000 | 0x7FFF8;
	pSrcBuffer = 0x80;
	HAL_SRAM_Write_8b(&hsram1, (uint32_t *)sramAddr, &pSrcBuffer, BufferSize);

	sramAddr = 0x60000000 | 0x7FFF9;
	pSrcBuffer = RTCopt.Seconds;
	HAL_SRAM_Write_8b(&hsram1, (uint32_t *)sramAddr, &pSrcBuffer, BufferSize);

	sramAddr = 0x60000000 | 0x7FFFa;
	pSrcBuffer = RTCopt.Minutes;
	HAL_SRAM_Write_8b(&hsram1, (uint32_t *)sramAddr, &pSrcBuffer, BufferSize);

	sramAddr = 0x60000000 | 0x7FFFb;
	pSrcBuffer = RTCopt.Hours;
	HAL_SRAM_Write_8b(&hsram1, (uint32_t *)sramAddr, &pSrcBuffer, BufferSize);

	sramAddr = 0x60000000 | 0x7FFFc;
	pSrcBuffer = 0x02;
	HAL_SRAM_Write_8b(&hsram1, (uint32_t *)sramAddr, &pSrcBuffer, BufferSize);

	sramAddr = 0x60000000 | 0x7FFFd;
	pSrcBuffer = RTCopt.Day;
	HAL_SRAM_Write_8b(&hsram1, (uint32_t *)sramAddr, &pSrcBuffer, BufferSize);

	sramAddr = 0x60000000 | 0x7FFFe;
	pSrcBuffer = RTCopt.Month;
	HAL_SRAM_Write_8b(&hsram1, (uint32_t *)sramAddr, &pSrcBuffer, BufferSize);

	sramAddr = 0x60000000 | 0x7FFFf;
	pSrcBuffer = RTCopt.YearII;
	HAL_SRAM_Write_8b(&hsram1, (uint32_t *)sramAddr, &pSrcBuffer, BufferSize);

	sramAddr = 0x60000000 | 0x7FFF8;
	pSrcBuffer = 0x00;
	HAL_SRAM_Write_8b(&hsram1, (uint32_t *)sramAddr, &pSrcBuffer, BufferSize);

}


