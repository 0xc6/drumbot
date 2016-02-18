/*
 * bit_array.c
 *
 *  Created on: 19.02.2013
 *      Author: cgries
 */

#include <stdint.h>

void bit_arr_clear(uint8_t* arr, uint8_t len) {
	for (uint8_t i = 0; i < len; i++) {
		arr[i] = 0x00;
	}
}

void bit_arr_set(uint8_t* arr, uint8_t bitno, uint8_t val) {
	if (val) {
		arr[bitno / 8] |= (1 << (bitno % 8));
	}
	else {
		arr[bitno / 8] &= ~(1 << (bitno % 8));
	}
}

uint8_t bit_arr_get(const uint8_t* arr, uint8_t const bitno) {
	return !! (arr[bitno / 8] & (1 << (bitno % 8)));
}

void bit_arr_toggle(uint8_t* arr, uint8_t bitno) {
	arr[bitno / 8] ^= (1 << (bitno % 8));
}


