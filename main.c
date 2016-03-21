/*
 * main.c
 *
 *  Created on: 11.01.2016
 *      Author: cgries
 */

#include <stdlib.h>
#include <avr/interrupt.h>
#include <avr/sleep.h>
#include <avr/io.h>

#include "timer.h"
#include "encoder.h"
#include "button.h"
#include "lcd.h"
#include "menu.h"
#include "main.h"

#include <avr/interrupt.h>
#include <util/atomic.h>
/**
 * current pin layout:
 *
 */



int main(void) {


	timer_init();
	button_init();
	encoder_init();
	
	menu_init();
	tracker_init();

	
	sei(); //globally enable interrupts



	while (1) {

		//see if we need to run any timer call-backs
		timer_check();
		

		//have any buttons been pressed?
		button_check();

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




