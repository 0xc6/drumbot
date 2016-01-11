/*
 * main.c
 *
 *  Created on: 20.07.2013
 *      Author: cgries
 */

#include <stdlib.h>
#include <util/delay.h>
#include <avr/interrupt.h>
#include <avr/sleep.h>
#include <avr/io.h>

#include "timer.h"
#include "button.h"
#include "main.h"
/**
 * current pin layout:
 *
 * PC5: relay out
 * PB1: code button in, PB2: action button in
 * PC2: Switch "door open" in
 * PC4: code led out, PC3: action led out
 *
 * PD0, PD1: password length DIP switches
 * PD2, PD3, PD4, PB7, PD5, PD6, PD7, PB0: code (off: dot, on: dash)
 */




int main(void) {


	timer_init();
	button_init();


	sei(); //globally enable interrupts

	DDRB |= (1 << PB0);
	PORTB &= ~(1 << PB0);

	while (1) {

		//see if we need to run any timer call-backs
		timer_check();

		//have any buttons been pressed?
		//button_check();

		//ok, sleep now
		//we need a special sequence of calls in order to ensure we miss no interrupts
		//see also http://www.nongnu.org/avr-libc/user-manual/group__avr__sleep.html
		cli();
		set_sleep_mode(SLEEP_MODE_IDLE);
		sleep_enable();
		sei();
		sleep_cpu();
		sleep_disable();

	}

	return 0;
}




