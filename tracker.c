#include "tracker.h"
#include "timer.h"
#include "bit_array.h"


struct tracker_data_t tracker_data = {
	.track = {
		{
			.note = {0b00000000, 0b00000000},
			.flags = {
				.is_enabled = 1
			}
		},
		{
			.note = {0b00000000, 0b00000000},
			.flags = {
				.is_enabled = 0
			}
		},
		{
			.note = {0b01000111, 0b00000001},
			.flags = {
				.is_enabled = 0
			}
		},
		{
			.note = {0b11100010, 0b01010101},
			.flags = {
				.is_enabled = 1
			}
		}
	},
	.position = 0
};

uint8_t tracker_get_note(const struct track_t* track, uint8_t note) {
	return bit_arr_get(track->note, note);
}

struct track_t* tracker_get_track(uint8_t track) {
	return &tracker_data.track[track];
};
	
void tracker_toggle_note(struct track_t* track, uint8_t note) {
	if (note < TRACKER_NOTES_PER_TRACK) 
	{
		return bit_arr_toggle(track->note, note);
	}
};

uint8_t tracker_get_position(void) {
	return tracker_data.position;
};

void tracker_set_position(uint8_t pos) {
	tracker_data.position = pos;
};

void tracker_set_bpm(int16_t bpm) {
	tracker_data.interval = (((uint16_t) 60000) / bpm);
}

uint16_t tracker_get_interval(void) {
	return tracker_data.interval;
}

void tracker_inc_position(void) {
	tracker_data.position++;
	tracker_data.position %= TRACKER_NOTES_PER_TRACK;
};

static void tracker_timer_cb (uint8_t timer_id) {
	timer_set(TIMER_TRACKER_BEAT, tracker_data.interval);
	tracker_inc_position();
};

void tracker_init(void) {
	timer_register(TIMER_TRACKER_BEAT, tracker_timer_cb);
	
	tracker_set_bpm(TRACKER_BPM_INITIAL);
	tracker_set_global_state(TRACKER_STATE_RUN);
};

void tracker_set_track_state(struct track_t* track, uint8_t track_state) {
	if (track_state == TRACKER_STATE_STOP) {
		track->flags.is_enabled = 0;
	}
	else if (track_state == TRACKER_STATE_STOP) {
		track->flags.is_enabled  = 1;
	}
	else { //toggle
		track->flags.is_enabled = !track->flags.is_enabled;
	}
}

uint8_t tracker_get_track_state(const struct track_t* track) {
	return track->flags.is_enabled ? TRACKER_STATE_RUN : TRACKER_STATE_STOP;
}

void tracker_set_global_state(uint8_t tracker_state) {
	if (tracker_state == TRACKER_STATE_STOP) {
		timer_stop(TIMER_TRACKER_BEAT);
		tracker_data.position = 0;
	}
	else {
		tracker_timer_cb(TIMER_TRACKER_BEAT);
	}
	for (uint8_t cur_track = 0; cur_track < TRACKER_NUMBER_OF_TRACKS; cur_track++) {
		tracker_data.track[cur_track].flags.is_enabled = tracker_state == TRACKER_STATE_RUN ? 1 : 0;
	}
}

