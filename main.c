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
//#include "encoder.h"
#include "lcd.h"
#include "main.h"
/**
 * current pin layout:
 *
 */

static uint8_t cur_char = 0x00;

void my_led_timer(uint8_t tmr_id) {
	PORTB ^= (1 << PB0);
	timer_set(TIMER_LED, 1000);
	
	lcd_putc(cur_char++);
}


int main(void) {

	lcd_init(LCD_DISP_ON);
	timer_init();
	//encoder_init();
	

	//testcode
	DDRB |= (1 << PB0);
	timer_register(TIMER_LED, &my_led_timer);
	my_led_timer(TIMER_LED);
	// end testcode
	
	sei(); //globally enable interrupts

	
	lcd_puts_P("Hello World");
	lcd_gotoxy(0,1);
	lcd_puts_P("Second line");


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




