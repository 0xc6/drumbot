/*
 * timer.h
 *
 *  Created on: 22.02.2013
 *      Author: cgries
 */

#ifndef TIMER_H_
#define TIMER_H_


struct timer_t { //auto-initialized
	volatile uint16_t tmr_val;
	volatile uint8_t tmr_active;
	void (*tmr_cb)(uint8_t id);
};

void timer_init();
void timer_register(uint8_t tmr_num, void (*cb)(uint8_t id));
void timer_set(uint8_t tmr_num, uint16_t val);
uint8_t timer_has_elapsed(uint8_t tmr_num);
uint16_t timer_stop(uint8_t tmr_num);
void timer_check();

#define NUM_TIMERS 4
#define TIMER_LED 0 //this timer is used to blink the red code led
#define TIMER_SM_TIMEOUT 1 //this timer is used for timeouts in the state machine
#define TIMER_BTN_CODE 2 //this timer measures for how long the code button has been pressed
#define TIMER_BTN_ACTION 3 //this timer measures for how long the action button has been pressed

#define TIMER_RESOLUTION 20 //milliseconds
#define TIMER_MS_TO_TICKS(v) ((v) / TIMER_RESOLUTION)
#define TIMER_TICKS_TO_MS(v) ((v) * TIMER_RESOLUTION)

#endif /* TIMER_H_ */
