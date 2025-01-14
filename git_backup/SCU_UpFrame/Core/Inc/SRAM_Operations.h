/*
 * SRAM_Operations.h
 *
 *  Created on: Nov 6, 2024
 *      Author: AEM
 */

#ifndef INC_SRAM_OPERATIONS_H_
#define INC_SRAM_OPERATIONS_H_

#define NAND_PAGE_SIZE 8192  // Define the NAND page size


/* External variables --------------------------------------------------------*/
extern SRAM_HandleTypeDef hsram1;  // Handle for SRAM
extern UART_HandleTypeDef huart5;  // Handle for UART
extern UART_HandleTypeDef huart4;  // Handle for UART
extern UART_HandleTypeDef huart8;  // Handle for UART
extern UART_HandleTypeDef huart10;  // Handle for UART
extern NAND_AddressTypeDef nand_address;  // NAND address structure
extern uint8_t time_set;  // Flag for setting time
extern uint8_t time_str[25];  // Time string
extern uint32_t sramAddr;  // SRAM address
//extern uint8_t check_channel ;
extern uint8_t nand_update;  // Flag for NAND update


typedef struct{

	uint16_t StartbyteI  :8;
	uint16_t StartbyteII :8;
	uint8_t Day;
	uint8_t Month;
	uint16_t YearI  	 :8;
	uint16_t YearII  	 :8;
	uint8_t Hours;
	uint8_t Minutes;
	uint8_t Seconds;
	uint16_t EndbyteI    :8;
	uint16_t EndbyteII   :8;
//	uint8_t RTC_flag;
}_sRTCOperation;

#pragma pack(1)

typedef struct{

	uint8_t StartbyteI;
	uint8_t StartbyteII;
	int16_t TSU_Buf[12];
	uint8_t Day;
	uint8_t Month;
	uint8_t YearI;
	uint8_t YearII;
	uint8_t Hours;
	uint8_t Minutes;
	uint8_t Seconds;
	uint8_t EndbyteI;
	uint8_t EndbyteII;
}_sTSUFrame;


typedef struct{

	uint8_t StartbyteI;
	uint8_t StartbyteII;
	float MCS_Buf[12];
	uint8_t Day;
	uint8_t Month;
	uint8_t YearI;
	uint8_t YearII;
	uint8_t Hours;
	uint8_t Minutes;
	uint8_t Seconds;
	uint8_t EndbyteI;
	uint8_t EndbyteII;
}_sMCSFrame;
#pragma pack()

typedef struct{

	uint32_t last_update_location;
	uint16_t sram_count;

}_sSRAM_LastAddr;

#endif /* INC_SRAM_OPERATIONS_H_ */
