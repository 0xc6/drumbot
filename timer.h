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
	volatile struct {
		uint8_t is_active : 1;
		uint8_t is_isr: 1;
	} flags;
	volatile uint8_t tmr_active;
	void (*tmr_cb)(uint8_t id);
};

void timer_init(void);
void timer_register(uint8_t tmr_num, uint8_t run_as_isr, void (*cb)(uint8_t id));
void timer_set(uint8_t tmr_num, uint16_t val);
uint8_t timer_has_elapsed(uint8_t tmr_num);
uint16_t timer_stop(uint8_t tmr_num);
void timer_check(void);

enum {
	TIMER_TRACKER_BEAT, //used for the drum track
	TIMER_TRACKER_RELEASE,  //used to release the solenoids after engaging 
	TIMER_BUTTON_ENCODER, //poll encoder button
	TIMER_BUTTON_MASTER_CONTROL, //start/stop button
	TIMER_MENU, // used to redraw the menu on the LCD

	TIMER_NUM_TIMERS, //how many timers there are
};


#define TIMER_RESOLUTION 10 //milliseconds
#define TIMER_MS_TO_TICKS(v) ((v) / TIMER_RESOLUTION)
#define TIMER_TICKS_TO_MS(v) ((v) * TIMER_RESOLUTION)

#endif /* TIMER_H_ */
