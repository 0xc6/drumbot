#include "menu.h"
#include "bit_array.h"
#include "encoder.h"
#include "tracker.h"

#include <stdlib.h>



static const __flash uint8_t menu_chars[DISP_NUM_SPECIAL_CHARS][DISP_CHAR_HEIGHT] = {
	{
		0b11111, //solid block
		0b11111,
		0b11111,
		0b11111,
		0b11111,
		0b11111,
		0b11111
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
		0b11111, //inverted pause
		0b11111,
		0b11111,
		0b00000,
		0b11111,
		0b11111,
		0b11111
	}
};



//functions for inputs

static void channel_draw(struct input_t* self) {
	struct channel_input_t* s = (struct channel_input_t*) self;

	DISP_GOTOXY(s->coord_x, s->coord_y);
	DISP_PUTS(s->label);
	DISP_PUTS_P(":  ");
	DISP_PUTC(DISP_SS_QSEP);
	
	for (uint8_t cur_note = 0; cur_note < TRACKER_NOTES_PER_TRACK; cur_note++) {
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
				lcd_putc(DISP_SS_PAUSE);
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




// functions for screens

static void screen_draw(struct screen_t* self) {

	struct input_t** input_p = self->input;
	
	do {
		(*input_p)->draw(*input_p);
	} while (*++input_p != NULL);

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
	channel_1.next_input = (struct input_t*) &channel_2;
	channel_1.prev_input = (struct input_t*) &channel_4;
	channel_1.on_focus = NULL;
	channel_1.on_blur = NULL;
	channel_1.draw = &channel_draw;
	channel_1.on_encoder_change = NULL;
	channel_1.on_btn_up = NULL;
	
	channel_2.coord_x = 0;
	channel_2.coord_y = 1;
	channel_2.label = "CH2";
	channel_2.track = tracker_get_track(1);
	channel_2.next_input = (struct input_t*) &channel_3;
	channel_2.prev_input = (struct input_t*) &channel_1;
	channel_2.on_focus = NULL;
	channel_2.on_blur = NULL;
	channel_2.draw = &channel_draw;
	channel_2.on_encoder_change = NULL;
	channel_2.on_btn_up = NULL;
	
	channel_3.coord_x = 0;
	channel_3.coord_y = 2;
	channel_3.label = "CH3";
	channel_3.track = tracker_get_track(2);
	channel_3.next_input = (struct input_t*) &channel_4;
	channel_3.prev_input = (struct input_t*) &channel_2;
	channel_3.on_focus = NULL;
	channel_3.on_blur = NULL;
	channel_3.draw = &channel_draw;
	channel_3.on_encoder_change = NULL;
	channel_3.on_btn_up = NULL;

	channel_4.coord_x = 0;
	channel_4.coord_y = 3;
	channel_4.label = "CH4";
	channel_4.track = tracker_get_track(3);
	channel_4.next_input = (struct input_t*) &channel_1;
	channel_4.prev_input = (struct input_t*) &channel_3;
	channel_4.on_focus = NULL;
	channel_4.on_blur = NULL;
	channel_4.draw = &channel_draw;
	channel_4.on_encoder_change = NULL;
	channel_4.on_btn_up = NULL;

	static struct input_t* track_screen_inputs[] = {
		(struct input_t*) &channel_1,
		(struct input_t*) &channel_2,
		(struct input_t*) &channel_3,
		(struct input_t*) &channel_4,
		NULL
	};


	static struct screen_t track_screen;
	track_screen.input = track_screen_inputs;
	track_screen.draw = &screen_draw;


	static struct screen_t *menu_screens[] = {
		&track_screen,
		NULL
	};


	menu.screen = menu_screens;
	menu.cur_screen = &track_screen;

}




// functions for menu_t

void menu_draw(void) {
	menu.cur_screen->draw(menu.cur_screen);
}


/*
void menu_redraw() {

	static int16_t enc_value;
	char enc_val_buf[8];

	//lcd_clrscr_all();
	for (uint8_t cur_track = 0; cur_track < TRACKER_NUMBER_OF_TRACKS; cur_track++) {
		DISP_GOTOXY(0, cur_track);
		lcd_puts_P("CH");
		lcd_putc('1' + (int8_t) cur_track);
		lcd_puts_P(":  ");
	
		lcd_putc(DISP_SS_QSEP);
		
		for (uint8_t cur_note = 0; cur_note < TRACKER_NOTES_PER_TRACK; cur_note++) {
			if (tracker_get_note(cur_track, cur_note)) {
				if (tracker_get_position() != cur_note) {
					lcd_putc(DISP_SS_NOTE);
				}
				else {
					lcd_putc(DISP_SS_BLOCK);
				}
			}
			else {
				if (tracker_get_position() != cur_note) {
					lcd_putc(DISP_SS_PAUSE);
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
	tracker_inc_position();
	//TESTING
	
	lcd_gotoxy(30, 0);
	enc_value -= encode_read1();
	ltoa(enc_value, enc_val_buf, 10);
	lcd_puts_P(" BPM: ");
	lcd_puts(enc_val_buf);

	//END TESTING

}
* */

void menu_init(void) {
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
	
}


