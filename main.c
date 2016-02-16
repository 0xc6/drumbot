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
#include "menu.h"
#include "main.h"

#include <avr/interrupt.h>
#include <util/atomic.h>
/**
 * current pin layout:
 *
 */



#define HIT_TIME 100

void my_test_timer(uint8_t tmr_id) {


	
}



#define lcd_e_high()    LCD_E_PORT  |=  _BV(LCD_E_PIN)
#define lcd_e_low()     LCD_E_PORT  &= ~_BV(LCD_E_PIN)
#define DDR(x) (*(&x - 1))

int main(void) {

	lcd_init(LCD_DISP_ON, 0);
	lcd_init(LCD_DISP_ON, 1);
	
	
	
	//DDR(LCD_E_PORT)     |= _BV(LCD_E_PIN);
	
	//lcd_e_low();
	lcd_gotoxy(0, 0);
	lcd_puts_P("Hello World");
	lcd_gotoxy(10, 1);
	lcd_puts_P("Hello World");
	lcd_gotoxy(20, 2);
	lcd_puts_P("Hello World");
	lcd_gotoxy(30, 3);
	lcd_puts_P("Hello World");
	
	
	//PORTC = 0;
	//PORTD = 0;
	
	//DDRC = 0; //(0 << PC1) | (0 << PC2);
	//DDRD = 0; //(1 << PD0) | (1 << PD1); 
	
	
//	timer_init();
//	encoder_init();
//	menu_init();
	

	//testcode
	
	//init LED blinker
//	DDRB |= (1 << PB0);
//	timer_register(TIMER_LED, &my_test_timer);
//	my_test_timer(TIMER_LED);
	
	
	
	
	// end testcode
	
	//sei(); //globally enable interrupts



	while (1) {

		//see if we need to run any timer call-backs
		//timer_check();
		
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




