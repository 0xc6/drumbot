/*
 * led.c
 *
 *  Created on: 22.07.2013
 *      Author: cgries
 */

#include <util/delay.h>
#include "led.h"


struct led_t my_leds[NUM_LEDS] = {
		{
				.id = LED_CODE,
				.pattern = LED_PATTERN_OFF,
				.port = &PORTC,
				.pin = PC4
		},
		{
				.id = LED_ACTION,
				.pattern = LED_PATTERN_OFF,
				.port = &PORTC,
				.pin = PC3
		}
};

static void led_update(uint8_t);

void led_init() {

	DDRC |= (1 << my_leds[0].pin) | (1 << my_leds[1].pin);

	timer_register(TIMER_LED, &led_update);
	timer_set(TIMER_LED, LED_FREQ);
}

void led_set(uint8_t led_id, uint8_t pattern) {
	for (uint8_t i = 0; i < NUM_LEDS; i++) {
		if (my_leds[i].id == led_id) {
			my_leds[i].pattern = pattern;
		}
	}
}

//use only for debug purposes, as _delay_ms is extremely code heavy
/*
void led_blink(uint8_t led_id, uint16_t duration) {
	*my_leds[led_id].port |= (1 << my_leds[led_id].pin);
	_delay_ms(duration);
	*my_leds[led_id].port &= ~(1 << my_leds[led_id].pin);
	_delay_ms(256);
}
*/

static void led_update(uint8_t timer_id) {
	for (uint8_t i = 0; i < NUM_LEDS; i++) {
		//the pattern is circularly bit-shifted to the left by one
		my_leds[i].pattern = (my_leds[i].pattern << 1) | (my_leds[i].pattern >> (sizeof(my_leds[i].pattern) * 8 - 1));

		//the led's value is set to the value of the lowest bit of the respective pattern
		*my_leds[i].port = (*my_leds[i].port & ~(1 << my_leds[i].pin)) | ((my_leds[i].pattern & 0x1) << my_leds[i].pin);
	}

	timer_set(TIMER_LED, LED_FREQ); //restart timer
}
