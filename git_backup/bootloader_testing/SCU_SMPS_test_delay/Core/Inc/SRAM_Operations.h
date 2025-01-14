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

	uint8_t Startbyte;
	uint8_t SRAM_buffer[350];
	uint8_t Endbyte;
	uint8_t SRAM_flag;
	uint16_t Idxcount;
}_sSRAMOperation;


typedef struct{

	uint32_t last_update_location;
	uint16_t sram_count;

}_sSRAM_LastAddr;

#endif /* INC_SRAM_OPERATIONS_H_ */
