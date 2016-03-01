#ifndef TRACKER_H_
#define TRACKER_H_
#include <stdint.h>
#include <avr/io.h>


#include "timer.h"
#include "bit_array.h"

// Tracker definitions

#define TRACKER_NUMBER_OF_TRACKS 4
#define TRACKER_NOTES_PER_TRACK 16

#define TRACKER_STATE_STOP 0
#define TRACKER_STATE_RUN 1
#define TRACKER_STATE_TOGGLE 2

#define TRACKER_BPM_INITIAL 100
#define TRACKER_BPM_MIN 30
#define TRACKER_BPM_MAX 150

#define TRACKER_HIT_TIME 100 //how long (ms) the actuators engage to play a note


struct tracker_data_t {
	struct track_t {
		uint8_t note[TRACKER_NOTES_PER_TRACK / sizeof(uint8_t)];
		void (*actuator_engage)(void);
		struct {
			uint8_t is_enabled: 1;
		} flags;
	} track[TRACKER_NUMBER_OF_TRACKS];
	struct {
		uint8_t is_enabled: 1;
	} flags;
	uint8_t position;
	uint8_t bpm;
	uint16_t interval; //bpm converted to milliseconds
};

extern struct track_t* tracker_get_track(uint8_t track);

extern uint8_t tracker_get_note(const struct track_t* track, uint8_t note);
extern void tracker_toggle_note(struct track_t* track, uint8_t note);
extern uint8_t tracker_get_position(void);

extern void tracker_set_position(uint8_t pos);
extern void tracker_inc_position(void);
extern void tracker_set_bpm(int16_t bpm);

extern void tracker_set_track_state(struct track_t* track, uint8_t track_state);
extern uint8_t tracker_get_track_state(const struct track_t* track);
extern void tracker_set_global_state(uint8_t tracker_state);

extern void tracker_init(void);


#endif // TRACKER_H_
