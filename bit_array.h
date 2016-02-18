/*
 * bit_array.h
 *
 *  Created on: 19.02.2013
 *      Author: cgries
 */

#ifndef BIT_ARRAY_H_
#define BIT_ARRAY_H_

extern void bit_arr_clear(uint8_t*, uint8_t);
extern void bit_arr_set(uint8_t*, uint8_t, uint8_t);
extern uint8_t bit_arr_get(uint8_t*, uint8_t);
extern void bit_arr_toggle(uint8_t*, uint8_t);

#endif /* BIT_ARRAY_H_ */
