/*
 * NAND_Operations.h
 *
 *  Created on: Nov 14, 2024
 *      Author: AEM
 */

#ifndef INC_NAND_OPERATIONS_H_
#define INC_NAND_OPERATIONS_H_

/*
 *  @315 for 15 days
 *  @630 for 30 days
 */
#define BLOCK_OFFSET	 630
// External handles for NAND and UART
extern NAND_HandleTypeDef hnand1;
extern UART_HandleTypeDef huart4;
extern UART_HandleTypeDef huart5;

#endif /* INC_NAND_OPERATIONS_H_ */
