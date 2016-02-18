#include "menu.h"
#include "bit_array.h"
#include "encoder.h"
#include "tracker.h"

#include <stdlib.h>



static const uint8_t menu_chars[DISP_NUM_SPECIAL_CHARS][DISP_CHAR_HEIGHT] PROGMEM = {
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
	




void menu_redraw() {

	static int16_t enc_value;
	char enc_val_buf[8];

	//lcd_clrscr_all();
	for (uint8_t cur_track = 0; cur_track < TRACKER_NUMBER_OF_TRACKS; cur_track++) {
		lcd_gotoxy(0, cur_track);
		lcd_puts_P("CH");
		lcd_putc('1' + (int8_t) cur_track);
		lcd_puts_P(":  ");
	
		lcd_putc(DISP_CHAR_QSEP);
		
		for (uint8_t cur_note = 0; cur_note < TRACKER_NOTES_PER_TRACK; cur_note++) {
			if (tracker_get_note(cur_track, cur_note)) {
				if (tracker_get_position() != cur_note) {
					lcd_putc(DISP_CHAR_NOTE);
				}
				else {
					lcd_putc(DISP_CHAR_BLOCK);
				}
			}
			else {
				if (tracker_get_position() != cur_note) {
					lcd_putc(DISP_CHAR_PAUSE);
				}
				else {
					lcd_putc(DISP_CHAR_BLOCK);
				}
			}
			
			if ((cur_note + 1) % 4 == 0) {
				lcd_putc(DISP_CHAR_QSEP);
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

void menu_init(void) {
	lcd_init(LCD_DISP_ON);

	for (uint8_t lcd_num = 0; lcd_num < LCD_NUM_LCDS; lcd_num++) {
		lcd_select_active_display(lcd_num);
		
		//load special chars into display
		for (uint8_t cur_char = 0; cur_char < DISP_NUM_SPECIAL_CHARS; cur_char++) {
			for (uint8_t cur_line = 0; cur_line < DISP_CHAR_HEIGHT; cur_line++) {
				lcd_command((1 << 6) | ((cur_char * (DISP_CHAR_HEIGHT+1)) + cur_line));
				lcd_data(pgm_read_byte(&menu_chars[cur_char][cur_line]));
			}
		}
				
		lcd_clrscr();
	}
	
	#define BEAT DISP_CHAR_NOTE
	
	/*
	lcd_gotoxy(0,0);
	lcd_puts_P("CH1:  |" BEAT "-" BEAT "-" "|" DISP_CHAR_INVNOTE BEAT "--" "|" "----" "|" "--" BEAT BEAT "|");
	lcd_gotoxy(0,1);
	lcd_puts_P("CH2:  |" BEAT BEAT BEAT BEAT "|" DISP_CHAR_INVPAUSE "--" BEAT "|" "-" BEAT BEAT "-" "|" BEAT "--" BEAT "|");
	lcd_gotoxy(0,2);
	lcd_puts_P("CH3:  |" BEAT "-" BEAT "-" "|" DISP_CHAR_INVNOTE BEAT "--" "|" "----" "|" "--" BEAT BEAT "|");
	lcd_gotoxy(0,3);
	lcd_puts_P("CH4:  |" BEAT BEAT BEAT BEAT "|" DISP_CHAR_INVPAUSE "--" BEAT "|" "-" BEAT BEAT "-" "|" BEAT "--" BEAT "|");
	*/
	

	
	
	
}