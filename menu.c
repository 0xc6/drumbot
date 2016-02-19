#include "menu.h"
#include "bit_array.h"
#include "encoder.h"
#include "tracker.h"

#include <stdlib.h>



static const __flash uint8_t menu_chars[DISP_NUM_SPECIAL_CHARS][DISP_CHAR_HEIGHT] = {
	{
		0b10000, //"play" symbol
		0b11000,
		0b11100,
		0b11110,
		0b11100,
		0b11000,
		0b10000
	},
	{
		0b00110, //musical note
		0b00101,
		0b00100,
		0b00110,
		0b11101,
		0b11100,
		0b11100
	},
		{
		0b11001, //inverted musical note
		0b11010,
		0b11011,
		0b11001,
		0b00010,
		0b00011,
		0b00011
	},
	{
		0b11111, //inverted "no note"
		0b11111,
		0b11111,
		0b00000,
		0b11111,
		0b11111,
		0b11111
	},
	{
		0b11011, //"pause" symbol
		0b11011,
		0b11011,
		0b11011,
		0b11011,
		0b11011,
		0b11011
	}
};


//menu_cursor is used to implement blinking cursors.
static uint8_t menu_cursor;
void update_cursor(void) {
	static uint8_t cursor_factor = 0;
	
	if (cursor_factor == 0) {
		menu_cursor = !menu_cursor;
	}
	
	cursor_factor++;
	cursor_factor %= MENU_CURSOR_BLINK_FACTOR;
}




//functions for inputs

static void channel_draw(struct input_t* self) {
	struct channel_input_t* s = (struct channel_input_t*) self;

	DISP_GOTOXY(s->coord_x, s->coord_y);
	DISP_PUTS(s->label);
	DISP_PUTS_P(":  ");
	
	if (s->cursor_pos == -1 && s->flags.is_focused && !menu_cursor) { //cursor on start/stop
		DISP_PUTC(' ');
	}
	else {
		if (s->track->flags.is_enabled) {
			DISP_PUTC(DISP_SS_PLAY);
		}
		else {
			DISP_PUTC(DISP_SS_PAUSE);
		}
	}
	DISP_PUTS_P(" |");
	
	for (uint8_t cur_note = 0; cur_note < TRACKER_NOTES_PER_TRACK; cur_note++) {
		if (cur_note == s->cursor_pos && s->flags.is_focused && !menu_cursor) {
			DISP_PUTC(' ');
		}
		else {
			if (tracker_get_note(s->track, cur_note)) {
				if (tracker_get_position() != cur_note) {
					lcd_putc(DISP_SS_NOTE);
				}
				else {
					lcd_putc(DISP_SS_BLOCK);
				}
			}
			else {
				if (tracker_get_position() != cur_note) {
					lcd_putc(DISP_SS_NO_NOTE);
				}
				else {
					lcd_putc(DISP_SS_BLOCK);
				}
			}
			
			if ((cur_note + 1) % 4 == 0) {
				lcd_putc(DISP_SS_QSEP);
			}
		}
	}

}


uint8_t channel_input_event(struct input_t* self, struct event_args_t* ev_args) {
	struct channel_input_t* s = (struct channel_input_t*) self;
	uint8_t ret = MENU_INPUT_EVENT_RESULT_NONE;
	
	
	switch (ev_args->ev_type) {
		case MENU_INPUT_EVENT_FOCUS_LEFT:
			s->flags.is_focused = 1;
			s->cursor_pos = 0;
		
			break;
		case MENU_INPUT_EVENT_ENCODER:
			s->cursor_pos += ev_args->args.encoder_event.increment;
			break;
		
	}
	
	return ret;
}

// functions for screens

static void screen_draw(struct screen_t* self) {

	struct input_t** input_p = self->input;
	
	do {
		(*input_p)->draw(*input_p);
	} while (*++input_p != NULL);

}
	
static uint8_t screen_event(struct screen_t* self, struct event_args_t* ev_args) {
	return self->focused_input->event(self->focused_input, ev_args);
}


// the big initialization function
static struct menu_t menu;
void menu_create(void) { 

	static struct channel_input_t channel_1;
	static struct channel_input_t channel_2;
	static struct channel_input_t channel_3;
	static struct channel_input_t channel_4 ;

	channel_1.coord_x = 0;
	channel_1.coord_y = 0;
	channel_1.label = "CH1";
	channel_1.track = tracker_get_track(0);
	channel_1.draw = &channel_draw;
	channel_1.event = &channel_input_event;
	
	channel_2.coord_x = 0;
	channel_2.coord_y = 1;
	channel_2.label = "CH2";
	channel_2.track = tracker_get_track(1);
	channel_2.draw = &channel_draw;
	channel_2.event = &channel_input_event;
	
	channel_3.coord_x = 0;
	channel_3.coord_y = 2;
	channel_3.label = "CH3";
	channel_3.track = tracker_get_track(2);
	channel_3.draw = &channel_draw;
	channel_3.event = &channel_input_event;

	channel_4.coord_x = 0;
	channel_4.coord_y = 3;
	channel_4.label = "CH4";
	channel_4.track = tracker_get_track(3);
	channel_4.draw = &channel_draw;
	channel_4.event = &channel_input_event;

	static struct input_t* track_screen_inputs[] = {
		(struct input_t*) &channel_1,
		(struct input_t*) &channel_2,
		(struct input_t*) &channel_3,
		(struct input_t*) &channel_4,
		NULL
	};


	static struct screen_t track_screen;
	track_screen.input = track_screen_inputs;
	track_screen.focused_input = track_screen_inputs[0];
	track_screen.event = &screen_event;
	track_screen.draw = &screen_draw;


	static struct screen_t *menu_screens[] = {
		&track_screen,
		NULL
	};

	menu.screen = menu_screens;
	menu.cur_screen = &track_screen;
}




// functions for menu_t



void menu_timer_cb(uint8_t timer_id) {
		PORTB ^= (1 << PB0);
	timer_set(TIMER_MENU, MENU_REFRESH_INTERVAL);
	
	int8_t enc_val = menu.encoder_fn();
	if (enc_val != 0) {
		struct event_args_t enc_ev = {
			.ev_type = MENU_INPUT_EVENT_ENCODER,
			.args.encoder_event.increment = enc_val
		};
		menu.cur_screen->event(menu.cur_screen, &enc_ev);
	}
	
	
	menu.cur_screen->draw(menu.cur_screen);
	update_cursor();
} 

void menu_init(int8_t (*encoder_fn)(void)) {
	lcd_init(LCD_DISP_ON);

	for (uint8_t lcd_num = 0; lcd_num < LCD_NUM_LCDS; lcd_num++) {
		lcd_select_active_display(lcd_num);
		
		//load special chars into display
		for (uint8_t cur_char = 0; cur_char < DISP_NUM_SPECIAL_CHARS; cur_char++) {
			for (uint8_t cur_line = 0; cur_line < DISP_CHAR_HEIGHT; cur_line++) {
				lcd_command((1 << 6) | ((cur_char * (DISP_CHAR_HEIGHT+1)) + cur_line));
				lcd_data(menu_chars[cur_char][cur_line]);
			}
		}
				
		lcd_clrscr();
	}
	
	menu_create();
	
	menu.encoder_fn = encoder_fn;
	
	//focus the first element on the screen
	auto struct event_args_t ev = {
		.ev_type = MENU_INPUT_EVENT_FOCUS_LEFT
	};
	menu.cur_screen->input[0]->event(menu.cur_screen->input[0], &ev);
	


	

		
	timer_register(TIMER_MENU, &menu_timer_cb);
	menu_timer_cb(TIMER_MENU);
	
}


