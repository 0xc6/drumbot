#include "tracker.h"


struct tracker_data_t tracker_data = {
	.track = {
		{
			.flags = {
				.is_enabled = 1
			}
		},
		{
			.flags = {
				.is_enabled = 0
			}
		},
		{
			.flags = {
				.is_enabled = 0
			}
		},
		{
			.flags = {
				.is_enabled = 1
			}
		}
	},
	.position = 0,
	.flags.is_enabled = 1
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
};

uint16_t tracker_get_interval(void) {
	return tracker_data.interval;
};

void tracker_inc_position(void) {
	tracker_data.position++;
	tracker_data.position %= TRACKER_NOTES_PER_TRACK;
};



static void tracker_timer_cb (uint8_t timer_id) {
	timer_set(TIMER_TRACKER_BEAT, tracker_data.interval);
	
	for (uint8_t i = 0; i < TRACKER_NUMBER_OF_TRACKS; i++) {
		struct track_t* cur_track = &tracker_data.track[i];
		if (tracker_get_note(cur_track, tracker_get_position())
			&& tracker_get_track_state(cur_track) == TRACKER_STATE_RUN) {
				cur_track->actuator_engage();
		}
	}
	timer_set(TIMER_TRACKER_RELEASE, TRACKER_HIT_TIME);
	tracker_inc_position();
};

static void tracker_track_1_actuator_engage(void) {
	PORTB |= (1 << PB0);
}
static void tracker_track_2_actuator_engage(void) {
	PORTD |= (1 << PD7);
}
static void tracker_track_3_actuator_engage(void) {
	PORTB |= (1 << PB2);
}
static void tracker_track_4_actuator_engage(void) {
	PORTB |= (1 << PB1);
}
static void tracker_actuators_disengage_cb(uint8_t timer_id) {
	PORTB &= ~((1 << PB0) | (1 << PB1) | (1 << PB2));
	PORTD &= ~(1 << PD7);
}



void tracker_init(void) {
	//ensure all actuator pins are set to low
	tracker_actuators_disengage_cb(TIMER_TRACKER_RELEASE);
	//then set them to output
	DDRB |= ((1 << PB0) | (1 << PB1) | (1 << PB2));
	DDRD |= (1 << PD7);
	
	timer_register(TIMER_TRACKER_BEAT, 0, tracker_timer_cb);
	//this timer runs in interrupt context to prevent any delays
	timer_register(TIMER_TRACKER_RELEASE, 1, &tracker_actuators_disengage_cb);
	
	tracker_data.track[0].actuator_engage = &tracker_track_1_actuator_engage;
	tracker_data.track[1].actuator_engage = &tracker_track_2_actuator_engage;
	tracker_data.track[2].actuator_engage = &tracker_track_3_actuator_engage;
	tracker_data.track[3].actuator_engage = &tracker_track_4_actuator_engage;
	
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

uint8_t tracker_get_global_state() {
	return tracker_data.flags.is_enabled ? TRACKER_STATE_RUN : TRACKER_STATE_STOP;
}

void tracker_set_global_state(uint8_t tracker_state) {
	if (tracker_state == TRACKER_STATE_TOGGLE) {
		if (tracker_data.flags.is_enabled) {
			tracker_set_global_state(TRACKER_STATE_STOP);
		}
		else {
			tracker_set_global_state(TRACKER_STATE_RUN);
		}
	}
	else {
		if (tracker_state == TRACKER_STATE_STOP) {
			timer_stop(TIMER_TRACKER_BEAT);
			tracker_data.position = 0;
			tracker_data.flags.is_enabled = 0;
		}
		else if (tracker_state == TRACKER_STATE_RUN) {
			tracker_timer_cb(TIMER_TRACKER_BEAT);
			tracker_data.flags.is_enabled = 1;
		}
		
		for (uint8_t cur_track = 0; cur_track < TRACKER_NUMBER_OF_TRACKS; cur_track++) {
			tracker_data.track[cur_track].flags.is_enabled = tracker_state == TRACKER_STATE_RUN ? 1 : 0;
		}
	}
}



