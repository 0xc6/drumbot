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
#include "lcd.h"
#include "menu.h"
#include "main.h"

#include <avr/interrupt.h>
#include <util/atomic.h>
/**
 * current pin layout:
 *
 */



/*
void my_test_timer(uint8_t tmr_id) {
	//timer_set(TIMER_TRACKER_BPM, 250);
	
	PORTB &= ~(1 << PB0);

	menu_draw();
	PORTB |= (1 << PB0);
	
}*/


int main(void) {

	DDRB |= (1 << PB0);


	timer_init();
	encoder_init();
	
	menu_init(&encode_read1);
	tracker_init();

	//testcode

	
	
	//init LED blinker
	
	//timer_register(TIMER_TRACKER_BPM, &my_test_timer);
	//my_test_timer(TIMER_TRACKER_BPM);
	
	
	
	
	// end testcode
	
	sei(); //globally enable interrupts



	while (1) {

		//see if we need to run any timer call-backs
		timer_check();
		
		//run encoder events
		//encoder_check();

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




