/*
 * led.h
 *
 *  Created on: 22.07.2013
 *      Author: cgries
 */

#ifndef LED_H_
#define LED_H_

#include <avr/io.h>
#include "timer.h"

#define NUM_LEDS 2
#define LED_CODE 0
#define LED_ACTION 1

//LED "frequency" in ms; a pattern is completed after 8*LED_FREQ ms
#define LED_FREQ 250

#define LED_PATTERN_OFF        0b00000000
#define LED_PATTERN_ON         0b11111111
#define LED_PATTERN_BLINK_SLOW 0b00001111
#define LED_PATTERN_BLINK_MED  0b00110011
#define LED_PATTERN_BLINK_FAST 0b01010101


struct led_t {
	uint8_t id;
	uint8_t pattern;
	volatile uint8_t* const port;
	const uint8_t pin;
};

void led_init();
void led_set(uint8_t, uint8_t);
void led_blink(uint8_t, uint16_t);


#endif /* LED_H_ */
