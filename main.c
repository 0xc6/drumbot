/*
 * main.c
 *
 *  Created on: 11.01.2016
 *      Author: cgries
 */

#include <stdlib.h>
#include <util/delay.h>
#include <avr/interrupt.h>
#include <avr/sleep.h>
#include <avr/io.h>

#include "timer.h"
#include "encoder.h"
#include "lcd.h"
#include "main.h"

#include <avr/interrupt.h>
#include <util/atomic.h>
/**
 * current pin layout:
 *
 */

static volatile int16_t enc_val = 0;
static char enc_val_buf[12];

void my_test_timer(uint8_t tmr_id) {
	
	enc_val += encode_read2();
	
	itoa(enc_val, enc_val_buf, 10);
	lcd_clrscr();
	lcd_puts(enc_val_buf);
	
	
	PORTB ^= (1 << PB0); //toggle LED
	timer_set(TIMER_LED, 100);
}



int main(void) {

	lcd_init(LCD_DISP_ON);
	timer_init();
	encoder_init();
	

	//testcode
	
	//init LED blinker
	DDRB |= (1 << PB0);
	timer_register(TIMER_LED, &my_test_timer);
	my_test_timer(TIMER_LED);
	
	
	
	
	// end testcode
	
	//sei(); //globally enable interrupts



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




