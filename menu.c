#include "menu.h"
#include "bit_array.h"
#include "encoder.h"
#include "button.h"
#include "tracker.h"

#include <stdlib.h>
#include <string.h>



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
	},
	{
		0b11011, //cursor symbol
		0b10001,
		0b00000,
		0b00000,
		0b00000,
		0b10001,
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

//channel input
#define MENU_CHANNEL_INPUT_BTN_START_STOP -1 //only used internally by channel input

//draw a single field (note or start/stop button) of a tracker input
static void channel_input_draw_single_field(struct channel_input_t* self, int8_t field_idx) {
	uint8_t x_pos = self->coord_x + strlen(self->label) + 1; //x offset + length of label + trailing space
	
	if (field_idx >= MENU_CHANNEL_INPUT_BTN_START_STOP && field_idx < TRACKER_NOTES_PER_TRACK) {
		//draw the start/stop button
		if (field_idx == MENU_CHANNEL_INPUT_BTN_START_STOP) {
			DISP_GOTOXY(x_pos, self->coord_y);
			
			if (self->cursor_pos == MENU_CHANNEL_INPUT_BTN_START_STOP && self->flags.is_focused && !menu_cursor) { //cursor on start/stop
				DISP_PUTC(DISP_SS_CURSOR);
			}
			else {
				if (self->track->flags.is_enabled) {
					DISP_PUTC(DISP_SS_PLAY);
				}
				else {
					DISP_PUTC(DISP_SS_PAUSE);
				}
			}
		}
		else { //draw note field
			x_pos 	+= 2 // start/stop symbol plus trailing space 
					+ field_idx //note offset
					+ (field_idx / 4) + 1; //account for separators after every 4 notes
			
			DISP_GOTOXY(x_pos, self->coord_y);

			uint8_t has_note = !! tracker_get_note(self->track, field_idx);
			uint8_t track_enabled = tracker_get_track_state(self->track) == TRACKER_STATE_RUN;
			uint8_t on_pos = tracker_get_position() == field_idx;
			
			if (field_idx == self->cursor_pos && self->flags.is_focused && menu_cursor) {
				DISP_PUTC(DISP_SS_CURSOR);
			}
			else {			
				if (has_note && (!track_enabled || !on_pos)) {
					DISP_PUTC(DISP_SS_NOTE);
				}
				else if (!has_note && (!track_enabled || !on_pos)) {
					DISP_PUTC(DISP_SS_NO_NOTE);
				}
				else if (track_enabled && on_pos) {
					DISP_PUTC(DISP_SS_BLOCK);
				}
			}
			
		}
	}
		
}

static void channel_input_draw(struct channel_input_t* self, uint8_t draw_full) {
	
	
	// draw the complete UI element
	if (draw_full) { 
		DISP_GOTOXY(self->coord_x, self->coord_y);
		DISP_PUTS(self->label);
		DISP_PUTS_P(" ");
		
		channel_input_draw_single_field(self, MENU_CHANNEL_INPUT_BTN_START_STOP);

		DISP_PUTC(' ');
		DISP_PUTC(DISP_SS_QSEP);
		
		for (uint8_t cur_note = 0; cur_note < TRACKER_NOTES_PER_TRACK; cur_note++) {
			
			channel_input_draw_single_field(self, cur_note);
			
			if ((cur_note + 1) % 4 == 0) {
				DISP_PUTC(DISP_SS_QSEP);
			}
		}
	}
	// only do a refresh, i.e. re-draw everything that might have changed.
	// this affects only five fields: old and new position of the cursor,
	// old and new tracker position and start/stop button.
	else {
		channel_input_draw_single_field(self, MENU_CHANNEL_INPUT_BTN_START_STOP);
		channel_input_draw_single_field(self, self->cursor_pos);
		channel_input_draw_single_field(self, self->last_cursor_pos);
		channel_input_draw_single_field(self, tracker_get_position());
		channel_input_draw_single_field(self, self->last_tracker_pos);
	}
	
	//remember the two values so that we can later re-draw these fields
	self->last_cursor_pos = self->cursor_pos;
	self->last_tracker_pos = tracker_get_position();
		
}


static uint8_t channel_input_event(struct channel_input_t* self, struct event_args_t* ev_args) {
	uint8_t ret = MENU_INPUT_EVENT_RESULT_NONE; //default result
	
	switch (ev_args->ev_type) {
		case MENU_INPUT_EVENT_FOCUS_LEFT:
			self->flags.is_focused = 1;
			self->cursor_pos = MENU_CHANNEL_INPUT_BTN_START_STOP;
			break;
			
		case MENU_INPUT_EVENT_FOCUS_RIGHT:
			self->flags.is_focused = 1;
			self->cursor_pos = TRACKER_NOTES_PER_TRACK - 1;
			break;
			
		case MENU_INPUT_EVENT_ENCODER:
			self->cursor_pos += ev_args->args.encoder_event.increment;
			
			if (self->cursor_pos > TRACKER_NOTES_PER_TRACK - 1) {
				ret = MENU_INPUT_EVENT_RESULT_BLUR_RIGHT;
				self->flags.is_focused = 0;
			}
			if (self->cursor_pos < MENU_CHANNEL_INPUT_BTN_START_STOP) {
				 ret = MENU_INPUT_EVENT_RESULT_BLUR_LEFT;
				 self->flags.is_focused = 0;
			}
			break;
		case MENU_INPUT_EVENT_BUTTON_UP:
			if (self->cursor_pos == MENU_CHANNEL_INPUT_BTN_START_STOP) {
				//can only control channel if tracker not globally stopped
				if (tracker_get_global_state() != TRACKER_STATE_STOP) {
					tracker_set_track_state(self->track, TRACKER_STATE_TOGGLE);
				}
			}
			else { //cursor on note
				tracker_toggle_note(self->track, self->cursor_pos);
			}
			break;
	}
	
	return ret;
}

//functions for number input
static void number_input_draw(struct number_input_t* self, uint8_t draw_full) {

	//only need to draw if focused or if draw_full is true
	if (self->flags.is_focused || self->flags.need_redraw || draw_full) {
		//how much space is reserved for drawing the number
		//warning: you may need to adjust this in accordance to min_value and max_value
		const uint8_t NUM_WIDTH = 3; 
		char num_buf[NUM_WIDTH + 1];
		
		memset(num_buf, ' ', NUM_WIDTH);
		num_buf[NUM_WIDTH] = '\0';
		
		DISP_GOTOXY(self->coord_x, self->coord_y);
		DISP_PUTS(self->label);
		DISP_PUTC(' ');
		
		uint8_t frame_visible = !self->flags.is_focused
			|| (self->flags.is_focused && menu_cursor)
			|| (self->flags.is_editing);
		uint8_t value_visible = !self->flags.is_editing 
			|| (self->flags.is_editing && menu_cursor);
		
		frame_visible ? DISP_PUTC('[') :DISP_PUTC(' ');
		
		if (value_visible) { 
			ltoa(self->value, num_buf, 10); //print number right aligned
			for (uint8_t i = 0; i < NUM_WIDTH - strlen(num_buf); i++) {
				DISP_PUTC(' ');
			}	
		}
		
		DISP_PUTS(num_buf);
		frame_visible ? DISP_PUTC(']') :DISP_PUTC(' ');
		self->flags.need_redraw = 0;
	}
}

static uint8_t number_input_event(struct number_input_t* self, struct event_args_t* ev_args) {
	uint8_t ret = MENU_INPUT_EVENT_RESULT_NONE; //default result
	
	switch (ev_args->ev_type) {
		case MENU_INPUT_EVENT_FOCUS_LEFT:
		case MENU_INPUT_EVENT_FOCUS_RIGHT:
			self->flags.is_focused = 1;
			break;
		case MENU_INPUT_EVENT_BUTTON_UP:
			self->flags.is_editing = !self->flags.is_editing;
			break;
		case MENU_INPUT_EVENT_ENCODER:
			if (self->flags.is_editing) {
				self->value += ev_args->args.encoder_event.increment;
				
				if (self->value < self->min_value) {
					self->value = self->min_value;
				}
				if (self->value > self->max_value) {
					self->value = self->max_value;
				}
				self->update(self->value);
			}
			else { //not editing -> will blur
				if (ev_args->args.encoder_event.increment < 0) {
					ret = MENU_INPUT_EVENT_RESULT_BLUR_LEFT;
				}
				else {// encoder turned right
					ret = MENU_INPUT_EVENT_RESULT_BLUR_RIGHT;
				}
				self->flags.is_focused = 0;
				self->flags.need_redraw = 1;
			}
			break;
	}
	return ret;
}



// functions for screens

static void screen_draw(struct screen_t* self, uint8_t draw_full) {

	struct input_t** input_p = self->input;
	
	do {
		(*input_p)->draw(*input_p, draw_full);
	} while (*++input_p != NULL);

}
	
static uint8_t screen_event(struct screen_t* self, struct event_args_t* ev_args) {
	uint8_t ev_result;

	//some events are handled by the screen class directly
	if (ev_args->ev_type == MENU_INPUT_EVENT_BUTTON_UP &&
		ev_args->args.button_event.btn_id == BUTTON_MASTER_CONTROL) {
			tracker_set_global_state(TRACKER_STATE_TOGGLE);
	}
	
	//all others are passed on to the focused input element
	else {
		//pass the event to the focused input
		ev_result = self->focused_input->event(self->focused_input, ev_args);

		//we might need to send a second event
		struct event_args_t my_ev;
		
		switch (ev_result) {
			case MENU_INPUT_EVENT_RESULT_BLUR_LEFT:
				my_ev.ev_type = MENU_INPUT_EVENT_FOCUS_RIGHT;
			
				self->focused_input = self->focused_input->prev_input;
				self->focused_input->event(self->focused_input, &my_ev);
			break;
			
			case MENU_INPUT_EVENT_RESULT_BLUR_RIGHT:
				my_ev.ev_type = MENU_INPUT_EVENT_FOCUS_LEFT;
				
				self->focused_input = self->focused_input->next_input;
				self->focused_input->event(self->focused_input, &my_ev);
			break;
		}
	}
	
	return MENU_INPUT_EVENT_RESULT_NONE;
}

// functions for the menu

static void menu_event(struct menu_t* self, struct event_args_t* ev_args) {
	// just forward it
	self->cur_screen->event(self->cur_screen, ev_args);
}

// the big initialization function
static struct menu_t menu;
void menu_create(void) { 

	static struct channel_input_t channel_1;
	static struct channel_input_t channel_2;
	static struct channel_input_t channel_3;
	static struct channel_input_t channel_4;
	static struct number_input_t bpm_input;
	
	channel_1.coord_x = 0;
	channel_1.coord_y = 0;
	channel_1.label = "CH1:";
	channel_1.track = tracker_get_track(0);
	channel_1.prev_input = (struct input_t*) &channel_4;
	channel_1.next_input = (struct input_t*) &bpm_input;
	channel_1.draw = (void (*)(struct input_t*, uint8_t)) &channel_input_draw;
	channel_1.event = (uint8_t (*)(struct input_t*, struct event_args_t*)) &channel_input_event;
	
	bpm_input.label = "BPM:";
	bpm_input.coord_x = 30;
	bpm_input.coord_y = 0;
	bpm_input.prev_input = (struct input_t*) &channel_1;
	bpm_input.next_input = (struct input_t*) &channel_2;
	bpm_input.draw = (void (*)(struct input_t*, uint8_t)) &number_input_draw;
	bpm_input.event = (uint8_t (*)(struct input_t*, struct event_args_t*)) &number_input_event;
	bpm_input.min_value = TRACKER_BPM_MIN;
	bpm_input.max_value = TRACKER_BPM_MAX;
	bpm_input.value = TRACKER_BPM_INITIAL;
	bpm_input.update = &tracker_set_bpm;
	
	
	channel_2.coord_x = 0;
	channel_2.coord_y = 1;
	channel_2.label = "CH2:";
	channel_2.track = tracker_get_track(1);
	channel_2.prev_input = (struct input_t*) &bpm_input;
	channel_2.next_input = (struct input_t*) &channel_3;
	channel_2.draw = (void (*)(struct input_t*, uint8_t)) &channel_input_draw;
	channel_2.event = (uint8_t (*)(struct input_t*, struct event_args_t*)) &channel_input_event;
	
	channel_3.coord_x = 0;
	channel_3.coord_y = 2;
	channel_3.label = "CH3:";
	channel_3.track = tracker_get_track(2);
	channel_3.prev_input = (struct input_t*) &channel_2;
	channel_3.next_input = (struct input_t*) &channel_4;
	channel_3.draw = (void (*)(struct input_t*, uint8_t)) &channel_input_draw;
	channel_3.event = (uint8_t (*)(struct input_t*, struct event_args_t*)) &channel_input_event;

	channel_4.coord_x = 0;
	channel_4.coord_y = 3;
	channel_4.label = "CH4:";
	channel_4.track = tracker_get_track(3);
	channel_4.prev_input = (struct input_t*) &channel_3;
	channel_4.next_input = (struct input_t*) &channel_1;
	channel_4.draw = (void (*)(struct input_t*, uint8_t)) &channel_input_draw;
	channel_4.event = (uint8_t (*)(struct input_t*, struct event_args_t*)) &channel_input_event;



	static struct input_t* track_screen_inputs[] = {
		(struct input_t*) &channel_1,
		(struct input_t*) &bpm_input,
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
	menu.event = &menu_event;
}




// functions for menu_t



void menu_timer_cb(uint8_t timer_id) {

	timer_set(TIMER_MENU, MENU_REFRESH_INTERVAL);
	
	int8_t enc_val = menu.encoder_fn();
	if (enc_val != 0) {
		struct event_args_t enc_ev = {
			.ev_type = MENU_INPUT_EVENT_ENCODER,
			.args.encoder_event.increment = enc_val
		};
		menu.event(&menu, &enc_ev);
	}
	
	
	menu.cur_screen->draw(menu.cur_screen, 0);
	update_cursor();
} 


void menu_on_button_up_cb(uint8_t button_id, uint16_t duration) {
	struct event_args_t btn_ev = {
		.ev_type = MENU_INPUT_EVENT_BUTTON_UP,
		.args.button_event = {
			.btn_id = button_id,
			.duration = duration
		}
	};
	menu.event(&menu, &btn_ev);
}

void menu_init() {
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
	
	//focus the first element on the screen
	auto struct event_args_t ev = {
		.ev_type = MENU_INPUT_EVENT_FOCUS_LEFT
	};
	menu.cur_screen->input[0]->event(menu.cur_screen->input[0], &ev);
	
	
	//set up menu event emitters
	menu.encoder_fn = &encode_read4;
	button_register(NULL, &menu_on_button_up_cb);
		
	timer_register(TIMER_MENU, 0, &menu_timer_cb);
	
	//draw screen completely (draw_full = 1)
	menu.cur_screen->draw(menu.cur_screen, 1);
	
	menu_timer_cb(TIMER_MENU);
	
}


