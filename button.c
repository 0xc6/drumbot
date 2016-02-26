/*
 * button.c
 *
 *  Created on: 20.07.2013
 *      Author: cgries
 */



#include <avr/io.h>
#include <stdlib.h>
#include "timer.h"
#include "button.h"

#define IS_BUTTON_DOWN_HIGH_ACTIVE(port, pin) ((port) & (1 << (pin)))
#define IS_BUTTON_DOWN_LOW_ACTIVE(port, pin) (! ((port) & (1 << (pin))))

static uint8_t is_btn_encoder_down(void) {
	return IS_BUTTON_DOWN_LOW_ACTIVE(PIND, PD4);
}
static uint8_t is_btn_master_ctrl_down(void) {
	return IS_BUTTON_DOWN_LOW_ACTIVE(PINB, PB4);
}


struct button_controller my_btn_ctrl = {
		.button = {
			{
				.id = BUTTON_ENCODER,
				.is_down = &is_btn_encoder_down,
				.timer = TIMER_BUTTON_ENCODER,
				.status = BS_UP
			},
			{
				.id = BUTTON_MASTER_CONTROL,
				.is_down = &is_btn_master_ctrl_down,
				.timer = TIMER_BUTTON_MASTER_CONTROL,
				.status = BS_UP
			}
		}
};

void button_init() {
	PORTD |= (1 << PD4); //pull-up encoder button
	PORTB |= (1 << PB4); //pull-up master control button
}

void button_register (
		void (*on_button_down_cb)(uint8_t),
		void (*on_button_up_cb)(uint8_t, uint16_t)
	) {
		my_btn_ctrl.on_button_down_cb = on_button_down_cb;
		my_btn_ctrl.on_button_up_cb = on_button_up_cb;
}

void button_check() {
	for (uint8_t bnum = 0; bnum < BUTTON_NUM_BUTTONS; bnum++) {
		struct button_t* cur_btn = &my_btn_ctrl.button[bnum];

		if (cur_btn->is_down()) {
			if (cur_btn->status == BS_UP) { // button was previously up, now the debounce period starts
				cur_btn->status = BS_DEBOUNCE;
				timer_set(cur_btn->timer, BUTTON_DEBOUNCE_TIME);
			}
		}
		else { //button is up
			if (cur_btn->status == BS_DOWN) { // button was previously pressed, now it's up
				uint16_t dwn_time = UINT16_MAX - timer_stop(cur_btn->timer); //stop measuring pressed time
				cur_btn->status = BS_UP;
				if (my_btn_ctrl.on_button_up_cb != NULL) {
					my_btn_ctrl.on_button_up_cb(cur_btn->id, dwn_time); //run the "button pressed" callback
				}
			}
			else if (cur_btn->status == BS_CANCELLED) {
				cur_btn->status = BS_UP;
			}
		}

		if (cur_btn->status == BS_DEBOUNCE && timer_has_elapsed(cur_btn->timer)) { //debounce time over
			if (cur_btn->is_down()) { //is the button /still/ pressed?
				timer_set(cur_btn->timer, UINT16_MAX); //start measuring pressed time
				cur_btn->status = BS_DOWN;
				if (my_btn_ctrl.on_button_down_cb != NULL) {
					my_btn_ctrl.on_button_down_cb(cur_btn->id); //run the "button pressed" callback
				}
			}
			else {
				cur_btn->status = BS_UP; //probably just a glitch, ignore it
			}
		}
	}
}

//Cancel all buttons, this is useful in cases where
//we want to make sure that currently pressed buttons create no "on button up"-
//event as soon as they are no longer pressed.
void button_clear_all() {
	for (uint8_t bnum = 0; bnum < BUTTON_NUM_BUTTONS; bnum++) {
		if (my_btn_ctrl.button[bnum].status != BS_UP) {
			my_btn_ctrl.button[bnum].status = BS_CANCELLED;
		}
	}
}

