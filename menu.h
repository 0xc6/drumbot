/*
 * menu.h
 *
 *  Created on: 11.01.2016
 *      Author: cgries
 */

#ifndef MENU_H_
#define MENU_H_

#include "lcd.h"
#include "tracker.h"

#define DISP_CHAR_BLOCK '\x08'
#define DISP_CHAR_NOTE '\x09'
#define DISP_CHAR_INVNOTE '\x02'
#define DISP_CHAR_PAUSE '-'
#define DISP_CHAR_INVPAUSE '\x03'
#define DISP_CHAR_QSEP '|' // quarter separator

#define DISP_CHAR_WIDTH 5
#define DISP_CHAR_HEIGHT 7
#define DISP_NUM_SPECIAL_CHARS 4


struct input_t {
	const uint8_t coord_x;
	const uint8_t coord_y;
	struct {
		uint8_t is_focused: 1;
		uint8_t is_editing: 1;
	} flags;

	struct input_t* right_input;
	struct input_t* left_input;	
	char* (*draw)(struct input_t* self);
	void (*on_encoder_change) (struct input_t* self, int8_t increment);
	void (*on_btn_up) (struct input_t* self, uint8_t btn, uint8_t duration);
	

	
};

//subclasses input_t
struct channel_input_t {
	struct input_t;
	const struct track_t *track;
	uint8_t cursor_pos;
};

//subclasses input_t
struct number_input_t {
	struct input_t;
	uint8_t value;
	uint8_t min_value;
	uint8_t max_value;
	 
};




struct screen_t {
	struct input_t **input;
	struct input* cur_input;
	
	char* (*draw)(void);
	void (*on_encoder_change) (int8_t increment);
	void (*on_btn_up) (uint8_t btn, uint8_t duration);
};

struct menu_t {
	struct screen_t **screen;
	struct screen_t* cur_screen;
	
	char* (*draw)(void);
	void (*on_encoder_change) (int8_t increment);
	void (*on_btn_up) (uint8_t btn, uint8_t duration);
};



extern void menu_init(void);

extern void menu_redraw(void);



#endif /* MENU_H_ */
