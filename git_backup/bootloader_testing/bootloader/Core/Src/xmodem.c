#include "stm32h5xx_hal.h"
#include <stdint.h>
#include <stdbool.h>
#include "xmodem.h"
#include "main.h"

#define SOH         0x01
#define EOT         0x04
#define ACK         0x06
#define NAK         0x15
#define PACKET_SIZE 128

//static uint8_t buffer[PACKET_SIZE];

int circular_buffer_is_empty(CircularBuffer *cb);
int circular_buffer_is_full(CircularBuffer *cb);
int circular_buffer_read_length(CircularBuffer *cb, uint8_t *dest, uint32_t length);
extern CircularBuffer uart_buffer;

/* CRC16 Calculation */
static uint16_t calculate_crc(const uint8_t *data, uint16_t length) {
	uint16_t crc = 0;
	for (uint16_t i = 0; i < length; i++) {
		crc ^= (data[i] << 8);
		for (uint8_t j = 0; j < 8; j++) {
			if (crc & 0x8000) {
				crc = (crc << 1) ^ 0x1021;
			} else {
				crc <<= 1;
			}
		}
	}
	return crc;
}

/* Receive a single XMODEM packet */
static bool xmodem_receive_packet(UART_HandleTypeDef *huart, uint8_t *packet, uint8_t *packet_num) {
	uint8_t header[3];
	uint16_t crc_received, crc_calculated;

	/* Wait for SOH */
	circular_buffer_read_length(&uart_buffer, &header[0], 3);
	if(header[0] != SOH){
		return false;
	}

	/* Read the packet data */
	if(circular_buffer_read_length(&uart_buffer, packet, 128) ){
		// return false;
	}

	/* Read the CRC */
	uint8_t crc_bytes[2];
	if(circular_buffer_read_length(&uart_buffer, crc_bytes, 2)) {
		//return false;
	}
	crc_received = (crc_bytes[0] << 8) | crc_bytes[1];

	/* Verify packet number and CRC */
	crc_calculated = calculate_crc(packet, PACKET_SIZE);
	if ((header[1] != (uint8_t)(~header[2])) || crc_calculated != crc_received) {
		return false;
	}

	*packet_num = header[1];
	return true;
}

/* XMODEM Receive Function */
bool xmodem_receive(UART_HandleTypeDef *huart) {
	uint32_t flash_address = FLASH_APP_START_ADDRESS;
	uint8_t packet_num = 1;
	uint8_t packet[PACKET_SIZE];
	bool transfer_active = false;
	/* Check for EOT */
	uint8_t eot;


	uint8_t response = 'C';
	HAL_UART_Transmit(huart, &response, 1, HAL_MAX_DELAY);
	transfer_active = true;
#if 0
 // Continuously send 'C' to request transfer until the first valid packet is received
	while (!transfer_active) {
		HAL_UART_Transmit(huart, &response, 1, HAL_MAX_DELAY);

		// Attempt to receive a packet
		if (xmodem_receive_packet(huart, packet, &packet_num)) {
			transfer_active = true; // Start transfer upon receiving the first valid packet
			break;
		}
	}
#endif

	while (transfer_active) {
		/* Send NAK to start the transfer */
		//        uint8_t response = NAK;
		//        HAL_UART_Transmit(huart, &response, 1, HAL_MAX_DELAY);
		HAL_Delay(100);
		/* Receive packets */
		if (xmodem_receive_packet(huart, packet, &packet_num)) {
			/* Write the packet to flash */
			for (uint32_t i = 0; i < PACKET_SIZE; i += 16) {
				// Prepare 16 bytes of data for FLASH_TYPEPROGRAM_QUADWORD
				uint64_t quadword[2]; // 16 bytes (128 bits)
				quadword[0] = *(uint64_t *)(packet + i);       // First 8 bytes
				quadword[1] = *(uint64_t *)(packet + i + 8);   // Next 8 bytes

				// Ensure the flash address is 16-byte aligned
				if (flash_address % 16 != 0) {
					// Handle alignment error (optional)
					break;
				}

				// Write 16 bytes to flash
				if (HAL_FLASH_Program(FLASH_TYPEPROGRAM_QUADWORD, flash_address, (uint32_t)quadword) != HAL_OK) {
					// Handle flash programming error
					break;
				}

				// Increment the address by 16 bytes for the next quadword
				flash_address += 16;
			}


			/* Send ACK */

			response = ACK;
			HAL_UART_Transmit(huart, &response, 1, HAL_MAX_DELAY);
			packet_num++;

		} else {
			/* Send NAK for invalid packet */
			response = NAK;
			HAL_UART_Transmit(huart, &response, 1, HAL_MAX_DELAY);
			circular_buffer_read_length(&uart_buffer, &eot,1);
			//            uart_transmit_str((uint8_t*)"\n\rSuccess.\n\r");
			//            uart_transmit_str((uint8_t*)"\n\rSuccess\n\r");
			if (eot != EOT)
				return false;
		}


		if (eot == EOT) {
			/* Send ACK and exit transfer */
			response = ACK;
			HAL_UART_Transmit(huart, &response, 1, HAL_MAX_DELAY);
			(void)uart_transmit_str((uint8_t*)"\n\rFirmware updated!\n\r");
			(void)uart_transmit_str((uint8_t*)"Switch to Normal Mode...\n\r");
//			(void)uart_transmit_str((uint8_t*)"Jumping to user application...\n\r");
//			flash_jump_to_app();
			transfer_active = false;
			while(1);
		}
	}

	return true;
}


int circular_buffer_read_length(CircularBuffer *cb, uint8_t *dest, uint32_t length) {
	if (circular_buffer_is_empty(cb)) {
		return false; // Buffer is empty
	}

	// Calculate available data
	uint32_t available_data = cb->full ? BUFFER_SIZE : (cb->head >= cb->tail ? cb->head - cb->tail : BUFFER_SIZE - (cb->tail - cb->head));
	if (length > available_data) {
		return false; // Not enough data
	}

	// Determine the number of bytes until the end of the buffer
	uint32_t bytes_to_end = BUFFER_SIZE - cb->tail;

	if (length <= bytes_to_end) {
		// All requested data is in a contiguous block
		memcpy(dest, &cb->data[cb->tail], length);
	} else {
		// Data wraps around, so split into two memcpy calls
		memcpy(dest, &cb->data[cb->tail], bytes_to_end);
		memcpy(dest + bytes_to_end, &cb->data[0], length - bytes_to_end);
	}

	// Advance the tail pointer
	cb->tail = (cb->tail + length) % BUFFER_SIZE;

	// Clear the full flag
	cb->full = false;

	return true;
}


int circular_buffer_is_full(CircularBuffer *cb) {
	return cb->full;
}

int circular_buffer_is_empty(CircularBuffer *cb) {
	return (cb->head == cb->tail && !cb->full);
}

