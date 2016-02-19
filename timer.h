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

void timer_init(void);
void timer_register(uint8_t tmr_num, void (*cb)(uint8_t id));
void timer_set(uint8_t tmr_num, uint16_t val);
uint8_t timer_has_elapsed(uint8_t tmr_num);
uint16_t timer_stop(uint8_t tmr_num);
void timer_check(void);

#define NUM_TIMERS 4
#define TIMER_TRACKER_BEAT 0 //used for the drum track
#define TIMER_TRACKER_RELEASE 1 //used to release the solenoids after engaging 
#define TIMER_PRIMARY_ENCODER 2 //used to poll the encoder
#define TIMER_MENU 3 // used to redraw the menu on the LCD

#define TIMER_RESOLUTION 10 //milliseconds
#define TIMER_MS_TO_TICKS(v) ((v) / TIMER_RESOLUTION)
#define TIMER_TICKS_TO_MS(v) ((v) * TIMER_RESOLUTION)

#endif /* TIMER_H_ */
