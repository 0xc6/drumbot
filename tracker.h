#ifndef TRACKER_H_
#define TRACKER_H_
#include <stdint.h>

// Tracker definitions

#define TRACKER_NUMBER_OF_TRACKS 4
#define TRACKER_NOTES_PER_TRACK 16

struct tracker_data_t{
	struct track_t {
		uint8_t note[TRACKER_NOTES_PER_TRACK / sizeof(uint8_t)];
		uint8_t is_enabled;
	} track[TRACKER_NUMBER_OF_TRACKS];
	uint8_t position;
};


extern uint8_t tracker_get_note(uint8_t track, uint8_t note);
extern struct track_t* tracker_get_track(uint8_t track);
extern void tracker_toggle_note(uint8_t track, uint8_t note);
extern uint8_t tracker_get_position(void);
extern void tracker_set_position(uint8_t pos);
extern void tracker_inc_position(void);


#endif // TRACKER_H_
