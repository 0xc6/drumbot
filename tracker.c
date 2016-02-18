#include "tracker.h"
#include "bit_array.h"


struct tracker_data_t tracker_data = {
	.track = {
		{
			.note = {0b00000000, 0b00000000},
		},
		{
			.note = {0b00000000, 0b00000000},
		},
		{
			.note = {0b01000111, 0b00000001},
		},
		{
			.note = {0b11100010, 0b01010101},
		}
	},
	.position = 0
};

uint8_t tracker_get_note(uint8_t track, uint8_t note) {
	return bit_arr_get(tracker_data.track[track].note, note);
}

struct track_t* tracker_get_track(uint8_t track) {
	return &tracker_data.track[track];
}
	
void tracker_toggle_note(uint8_t track, uint8_t note) {
	return bit_arr_toggle(tracker_data.track[track].note, note);
}

uint8_t tracker_get_position(void) {
	return tracker_data.position;
}

void tracker_set_position(uint8_t pos) {
	tracker_data.position = pos;
}

void tracker_inc_position(void) {
	tracker_data.position++;
	tracker_data.position %= TRACKER_NOTES_PER_TRACK;
}
