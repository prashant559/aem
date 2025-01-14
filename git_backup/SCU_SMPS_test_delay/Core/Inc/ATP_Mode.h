/*
 * ATP_Mode.h
 *
 *  Created on: Nov 6, 2024
 *      Author: AEM
 */

#ifndef INC_ATP_MODE_H_
#define INC_ATP_MODE_H_

#include"SRAM_Operations.h"

#define MAX_BLOCK 4095
#define MAX_PAGE 127
#define SoF					'@'
#define EoF					'$'


extern void time_setter();
extern void ATP_operation();
extern uint8_t ADC_operation();
extern uint8_t Init_NAND_addr();
extern float temp_sensor[3][4];
extern uint16_t sensor_hex[3][4];
extern void NAND_operation(void);
extern void update_NAND_addr();
extern uint8_t get_NAND_stored_data();
extern uint8_t get_NAND_address(void);
extern void get_NAND_block_status();
extern void get_NAND_data(uint8_t *sram_data);
extern uint16_t ADC_READ(uint8_t adc_num,uint8_t channel);
extern void update_NAND_CyclicAddr();
extern uint8_t get_NAND_CyclicAddr(void);
extern void update_SRAM_Addr();
extern uint32_t get_SRAM_Addr(void);
extern uint32_t sram_base_location;
extern uint32_t sramAddr_base_addr;
extern uint8_t SRAM_operation();
extern NAND_AddressTypeDef nand_address;
extern NAND_HandleTypeDef hnand1;
extern SRAM_HandleTypeDef hsram1;
extern UART_HandleTypeDef huart5;
extern UART_HandleTypeDef huart8;
extern UART_HandleTypeDef huart4;
extern UART_HandleTypeDef huart10;

extern char nand_stop_read_CMD[];

/*
 * variables declaration for NAND Flash
 */
extern uint8_t NAND_Buf[15];
extern uint8_t NAND_index;
extern uint8_t NAND_flag;
extern uint8_t nand_cyclic_run;
extern uint16_t size_counter;
//extern _sSRAM_LastAddr update_loc;
extern uint32_t sram_location ;

/*
 * Using this macro in ADC_Operation.c
 * Check actual values come form ADC
 */
#if 0
#define ADC_RAW_DATA
#endif

/*
 * NOt In UsE
 */
#if 0
#define NandUART
#endif

/*
 * Using this macro in main.c for complete NAND erase
 */
#if 1
#define FUll_NAND_Er
#endif

/*
 * Using this macro in ADC_Operation.c
 * Check condition for Wire Break of the sensors
 */
#if 1
#define WIREBREAK
#endif

/*
 * Using this macro in ADC_Operation.c
 * Check condition for Random values come from hardware
 */
#if 1
#define RAND_VAL_CHK
#endif


typedef enum
{
	LOW,
	HIGH,
}status;


/*
 * This is ADC channels,
 * using in AdcSensor_Avg() function
 */
typedef enum
{
	channel_1,
	channel_2,
	channel_3,
	channel_4,
}adcchannel;


typedef enum
{
	adc_1,
	adc_2,
	adc_3,
}adc;

typedef enum
{
	Second,
	Minute,
	Hour,
	Day,
	Date,
	Month,
	Year,
}rtc;


typedef enum
{
	POWER_CTRL_1=1,
	POWER_CTRL_2,
	INPUT_GPIO_1,
	INPUT_GPIO_2,
	INPUT_GPIO_3,
	INPUT_GPIO_4,
	INPUT_GPIO_5,
	OUTPUT_GPIO_1,
	OUTPUT_GPIO_2,
	TS_1,
	TS_2,
	TS_3,
	TS_4,
	TS_5,
	TS_6,
	TS_7,
	TS_8,
	TS_9,
	TS_10,
	TS_11,
	TS_12,
	RTC_CHECK,
	NAND_FLASH_WRITE,
	NAND_FLASH_READ,
	NAND_FLASH_ERASE,
	LoopBack_RS232,
	LoopBack_RS422,
	LoopBack1_RS485,
	LoopBack2_RS485,

}ATP_Operation;


#endif /* INC_ATP_MODE_H_ */
