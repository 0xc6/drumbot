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
#include <avr/pgmspace.h>


#define DISP_SS_BLOCK '\xFF' //special chars
#define DISP_SS_NOTE '\x09'
#define DISP_SS_INVNOTE '\x02'
#define DISP_SS_NO_NOTE '-'
#define DISP_SS_INVPAUSE '\x03'
#define DISP_SS_QSEP '|' // quarter separator for tracks
#define DISP_SS_PLAY '\x08'
#define DISP_SS_PAUSE '\x0C'

#define DISP_CHAR_WIDTH 5
#define DISP_CHAR_HEIGHT 7
#define DISP_NUM_SPECIAL_CHARS 5

#define DISP_GOTOXY(x,y) lcd_gotoxy(x,y)
#define DISP_PUTC(c) lcd_putc(c)
#define DISP_PUTS(s) lcd_puts(s)
#define DISP_PUTS_P(s) lcd_puts_P(s)

#define MENU_REFRESH_INTERVAL 333 //how often is the menu redrawn (ms)
#define MENU_CURSOR_BLINK_FACTOR 1 //every X times the display is refreshed the cursor should change state

#define MENU_INPUT_EVENT_ENCODER 		0
#define MENU_INPUT_EVENT_BUTTON 		1
#define MENU_INPUT_EVENT_FOCUS_LEFT		2
#define MENU_INPUT_EVENT_FOCUS_RIGHT 	3

#define MENU_INPUT_EVENT_RESULT_NONE		0
#define MENU_INPUT_EVENT_RESULT_BLUR_LEFT  	1
#define MENU_INPUT_EVENT_RESULT_BLUR_RIGHT 	2



struct event_args_t {
	uint8_t ev_type;
	union {
		struct {
			int8_t increment;
		} encoder_event;
		struct {
			uint8_t btn_id;
			uint8_t duration;
		} button_event;
	} args;
};

struct input_t {
	uint8_t coord_x;
	uint8_t coord_y;
	struct {
		uint8_t is_focused: 1;
		uint8_t is_editing: 1;
	} flags;
	
	void (*draw)(struct input_t* self);
	uint8_t (*event) (struct input_t* self, struct event_args_t* ev_args);
};

//subclasses input_t
struct channel_input_t {
	struct input_t;
	const struct track_t *track;
	char* label;
	int8_t cursor_pos;
};

//subclasses input_t
struct number_input_t {
	struct input_t;
	char* label;
	uint8_t value;
	uint8_t min_value;
	uint8_t max_value;
};



struct screen_t {
	struct input_t **input;
	struct input_t* focused_input;
	
	void (*draw)(struct screen_t* self);
	uint8_t (*event) (struct screen_t* self, struct event_args_t* ev_args);
};

struct menu_t {
	struct screen_t **screen;
	struct screen_t* cur_screen;
	int8_t (*encoder_fn) (void);
	
	void (*event) (struct input_t* self, struct event_args_t* ev_args);
	void (*on_btn_up) (uint8_t btn, uint8_t duration);
};



extern void menu_init(int8_t (*encoder_fn)(void));

extern void menu_redraw(void);



#endif /* MENU_H_ */
