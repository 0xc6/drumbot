/*
 * button.h
 *
 *  Created on: 20.07.2013
 *      Author: cgries
 */

#ifndef BUTTON_H_
#define BUTTON_H_


#define BUTTON_DEBOUNCE_TIME 20 //ms

enum {
	BUTTON_ENCODER,
	BUTTON_MASTER_CONTROL,
	
	BUTTON_NUM_BUTTONS
};



struct button_controller {
	struct button_t {
		const uint8_t id;

		uint8_t (*is_down)(void);
		const uint8_t timer; //this timer is responsible for debouncing/timing this button
		enum {BS_UP, BS_DEBOUNCE, BS_DOWN, BS_CANCELLED} status;
	} button [BUTTON_NUM_BUTTONS];

	void (*on_button_down_cb)(uint8_t button_id);
	void (*on_button_up_cb)(uint8_t button_id, uint16_t duration);
};

extern void button_init(void);
extern void button_register(void (*on_button_down_cb)(uint8_t), void (*on_button_up_cb)(uint8_t, uint16_t));
extern void button_clear_all(void);
extern void button_check(void);


#endif /* BUTTON_H_ */
