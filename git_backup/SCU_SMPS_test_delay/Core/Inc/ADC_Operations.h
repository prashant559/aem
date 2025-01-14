/*
 * ADC_Operations.h
 *
 *  Created on: Nov 11, 2024
 *      Author: AEM
 */

#ifndef INC_ADC_OPERATIONS_H_
#define INC_ADC_OPERATIONS_H_

#include"ATP_Mode.h"


#define ADC_MAX_VAL			26214
#define ADC_MIN_VAL 		9175//8192
#define WIREBREAK

// External SPI and SRAM handles, needed for SPI communication and SRAM operations
extern SPI_HandleTypeDef hspi2;
extern SPI_HandleTypeDef hspi3;
extern SPI_HandleTypeDef hspi5;
extern SRAM_HandleTypeDef hsram1;

// External UART handles for UART communication
extern UART_HandleTypeDef huart4;
extern UART_HandleTypeDef huart5;
extern UART_HandleTypeDef huart8;
extern UART_HandleTypeDef huart10;

typedef enum{
	ADC_1,
	ADC_2,
	ADC_3
}adc_var;

typedef enum{
	CHANNEL_1,
	CHANNEL_2,
	CHANNEL_3,
	CHANNEL_4
}channel_var;

#endif /* INC_ADC_OPERATIONS_H_ */
