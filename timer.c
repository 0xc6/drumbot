/*
 * timer.c
 *
 *  Created on: 22.02.2013
 *      Author: cgries
 */

#include <stdlib.h>
#include <stdint.h>
#include <avr/io.h>
#include <avr/interrupt.h>
#include <util/atomic.h>
#include "timer.h"


struct timer_t timer_data[TIMER_NUM_TIMERS];

void timer_init(void) {
	TCNT2 = 0x00; //counter value to zero
	OCR2 = 37; //Output Compare Register preset
	TIMSK |= (1 << OCIE2); //enable the output compare interrupt
	//Timer Control = CTC mode, prescaler = 256 (this starts the timer)
	TCCR2 = (1 << WGM21) | (1 << CS22) | (1 << CS21);
}

void timer_register(uint8_t tmr_num, uint8_t run_as_isr, void (*cb)(uint8_t id)) {
	timer_data[tmr_num].tmr_cb = cb;
	timer_data[tmr_num].flags.is_isr = run_as_isr;
}

void timer_set(uint8_t tmr_num, uint16_t val) {
	ATOMIC_BLOCK(ATOMIC_RESTORESTATE) {
		timer_data[tmr_num].tmr_val = TIMER_MS_TO_TICKS(val);
		timer_data[tmr_num].flags.is_active = 1;
	}
}

uint16_t timer_stop(uint8_t tmr_num) {
	uint16_t tval;
	ATOMIC_BLOCK(ATOMIC_RESTORESTATE) {
		timer_data[tmr_num].flags.is_active = 0;
		tval = TIMER_TICKS_TO_MS(timer_data[tmr_num].tmr_val); //return the remaining time
	}
	return tval;
}

uint8_t timer_has_elapsed(uint8_t tmr_num) {
	uint8_t elapsed;
	ATOMIC_BLOCK(ATOMIC_RESTORESTATE) {
		elapsed = timer_data[tmr_num].tmr_val == 0;
	}
	return elapsed;
}

void timer_check(void) {
	uint8_t run_cb;

	for (uint8_t i = 0; i < TIMER_NUM_TIMERS; i++) {

	    ATOMIC_BLOCK(ATOMIC_RESTORESTATE) {
	    	run_cb = timer_data[i].tmr_val == 0  //timer just elapsed
					&& timer_data[i].flags.is_active;
	    }

		if (run_cb) {
			timer_data[i].flags.is_active = 0;
			//it's allowed to define no callback
			if (timer_data[i].tmr_cb != NULL) {
				timer_data[i].tmr_cb(i);
			}
		}
	}
}


//triggers every TIMER_RESOLUTION ms
ISR(TIMER2_COMP_vect) {
	for (uint8_t i = 0; i < TIMER_NUM_TIMERS; i++) {
		if (timer_data[i].tmr_val > 0) {
			timer_data[i].tmr_val--;
		}
		//run the callback in interrupt context if specified
		else if (timer_data[i].flags.is_isr && timer_data[i].flags.is_active) {
			if (timer_data[i].tmr_cb != NULL) {
				//it's *NOT* allowed to define no callback in ISR context
				timer_data[i].tmr_cb(i);
			}
			timer_data[i].flags.is_active = 0;
		}
	}
}




