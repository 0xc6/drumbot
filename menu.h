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

#define DISP_SS_BLOCK '\x08' //special chars
#define DISP_SS_NOTE '\x09'
#define DISP_SS_INVNOTE '\x02'
#define DISP_SS_PAUSE '-'
#define DISP_SS_INVPAUSE '\x03'
#define DISP_SS_QSEP '|' // quarter separator

#define DISP_CHAR_WIDTH 5
#define DISP_CHAR_HEIGHT 7
#define DISP_NUM_SPECIAL_CHARS 4

#define DISP_GOTOXY(x,y) lcd_gotoxy(x,y)
#define DISP_PUTC(c) lcd_putc(c)
#define DISP_PUTS(s) lcd_puts(s)
#define DISP_PUTS_P(s) lcd_puts_P(s)

struct input_t {
	uint8_t coord_x;
	uint8_t coord_y;
	struct {
		uint8_t is_focused: 1;
		uint8_t is_editing: 1;
	} flags;

	struct input_t* prev_input;
	struct input_t* next_input;	
	void (*draw)(struct input_t* self);
	void (*on_focus) (struct input_t* prev_input);
	void (*on_blur) (void);
	void (*on_encoder_change) (struct input_t* self, int8_t increment);
	void (*on_btn_up) (struct input_t* self, uint8_t btn, uint8_t duration);
};

//subclasses input_t
struct channel_input_t {
	struct input_t;
	const struct track_t *track;
	char* label;
	uint8_t cursor_pos;
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
	struct input* cur_input;
	
	void (*draw)(struct screen_t* self);
	void (*on_encoder_change) (int8_t increment);
	void (*on_btn_up) (uint8_t btn, uint8_t duration);
};

struct menu_t {
	struct screen_t **screen;
	struct screen_t* cur_screen;
	
	void (*on_encoder_change) (int8_t increment);
	void (*on_btn_up) (uint8_t btn, uint8_t duration);
};



extern void menu_init(void);

extern void menu_redraw(void);



#endif /* MENU_H_ */
