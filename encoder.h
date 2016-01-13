/*
 * encoder.h
 *
 *  Created on: 11.01.2016
 *      Author: cgries
 */

#include <stdlib.h>
#include <stdint.h>
#include <avr/io.h>
#include <avr/interrupt.h>
#include <util/atomic.h>
#include "timer.h"

#ifndef ENCODER_H_
#define ENCODER_H_

#define NUM_ENCODERS 1
#define PRIMARY_ENCODER 0

struct encoder_controller_t {
	struct encoder_t {
		const uint8_t id;

		uint8_t (*phase_a_get_status)();
		uint8_t (*phase_b_get_status)();
		const uint8_t timer; //this timer is responsible for polling this encoder
		int16_t delta; 
		
	} encoder[NUM_ENCODERS];

	void (*on_value_update)(uint8_t encoder_id);

};


#endif /* ENCODER_H_ */
