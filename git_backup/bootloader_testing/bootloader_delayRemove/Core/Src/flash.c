/**
 * @file    flash.c
 * @author  Ferenc Nemeth
 * @date    21 Dec 2018
 * @brief   This module handles the memory related functions.
 *
 *          Copyright (c) 2018 Ferenc Nemeth - https://github.com/ferenc-nemeth
 */

#include "flash.h"
#include "stm32h573xx.h"
#include "main.h"
#include "stdio.h"

#define FLASH_BASE_BANK1          0x08000000  // Base address of Flash Bank 1
#define FLASH_BASE_BANK2          0x08100000  // Base address of Flash Bank 2
//#define FLASH_SECTOR_SIZE         0x2000      // 8 KB per sector
#define FLASH_BANK1_END_ADDRESS   0x080FFFFF  // End address of Flash Bank 1
#define FLASH_BANK2_END_ADDRESS   0x081FFFFF  // End address of Flash Bank 2
#define FLASH_TOTAL_SECTORS       (256)       // Total sectors in both banks (2 MB / 8 KB)
#define MAX_SECTOR_NUMBER  64      // Total number of sectors
/* Function pointer for jumping to user application. */
typedef void (*fnc_ptr)(void);




// Function to calculate sector and bank from address
void GetSectorAndBank(uint32_t address, uint32_t *sector, uint32_t *bank) {
    if (address < FLASH_BASE_BANK2) {
        // Address is in Bank 1
        *bank = FLASH_BANK_1;
        *sector = (address - FLASH_BASE_BANK1) / FLASH_SECTOR_SIZE;
    } else {
        // Address is in Bank 2
        *bank = FLASH_BANK_2;
        *sector = (address - FLASH_BASE_BANK2) / FLASH_SECTOR_SIZE;
    }
}

/**
 * @brief   This function erases the memory.
 * @param   address: First address to be erased (the last is the end of the flash).
 * @return  status: Report about the success of the erasing.
 */
flash_status flash_erase(uint32_t address) {

	HAL_FLASH_Unlock();

	FLASH_EraseInitTypeDef eraseInitStruct = {0};
	uint32_t pageError = 0;
	uint32_t sector, bank;
	while(address < 0x08100000)
	{
		// Calculate sector and bank from the address
		GetSectorAndBank(address, &sector, &bank);

		// Validate sector and address range
		if (sector >= FLASH_TOTAL_SECTORS || address > FLASH_BANK2_END_ADDRESS) {
			HAL_FLASH_Lock();
			return FLASH_ERROR;
		}

		// Configure the erase structure
		eraseInitStruct.TypeErase = FLASH_TYPEERASE_SECTORS;
		eraseInitStruct.Banks = bank;
		eraseInitStruct.Sector = sector;
		eraseInitStruct.NbSectors = 1;  // Erase one sector at a time

		// Perform Flash erase
		__disable_irq();

		if (HAL_FLASHEx_Erase(&eraseInitStruct, &pageError) != HAL_OK) {
			__enable_irq();
			HAL_FLASH_Lock();
			return FLASH_ERROR;
		}

		__enable_irq();
		address = address + 0x2000;
	}

	HAL_FLASH_Lock();
	return FLASH_OK;
}

/**
 * @brief   This function flashes the memory.
 * @param   address: First address to be written to.
 * @param   *data:   Array of the data that we want to write.
 * @param   *length: Size of the array.
 * @return  status: Report about the success of the writing.
 */
flash_status flash_write(uint32_t address, uint32_t *data, uint32_t length) {
	flash_status status = FLASH_OK;

	HAL_FLASH_Unlock();

	/* Loop through the array. */
	for (uint32_t i = 0u; (i < length) && (FLASH_OK == status); i++) {
		/* If we reached the end of the memory, then report an error and don't do anything else.*/
		if (FLASH_APP_END_ADDRESS <= address) {
			status |= FLASH_ERROR_SIZE;
		} else {
			/* The actual flashing. If there is an error, then report it. */
			if (HAL_OK
					!= HAL_FLASH_Program(FLASH_TYPEPROGRAM_QUADWORD, address,
							data[i])) {
				status |= FLASH_ERROR_WRITE;
			}
			/* Read back the content of the memory. If it is wrong, then report an error. */
			if (((data[i])) != (*(volatile uint32_t*) address)) {
				status |= FLASH_ERROR_READBACK;
			}

			/* Shift the address by a word. */
			address += 4u;
		}
	}

	HAL_FLASH_Lock();

	return status;
}



/**
 * @brief   Actually jumps to the user application.
 * @param   void
 * @return  void
 */
void flash_jump_to_app(void) {
	//	 Function pointer to the address of the user application.
	fnc_ptr jump_to_app;
	jump_to_app =(fnc_ptr) (*(volatile uint32_t*) (FLASH_APP_START_ADDRESS + 4u));

	HAL_DeInit();
	//	 Change the main stack pointer.
	__set_MSP(*(volatile uint32_t*) FLASH_APP_START_ADDRESS);
	jump_to_app();
}

