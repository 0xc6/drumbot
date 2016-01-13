/*
 * encoder.c
 *
 *  Created on: 11.01.2016
 *      Author: cgries
 */


#include "encoder.h"

#define GET_PHASE_STATUS(port, pin) ((port) & (1 << (pin)))

static uint8_t primary_enc_phase_a_get_status() {
	return GET_PHASE_STATUS(PINC, PC0);
}
static uint8_t primary_enc_phase_b_get_status() {
	return GET_PHASE_STATUS(PINC, PC1);
}

struct encoder_controller_t encoder_ctrl = {
	.encoder = {
		{
			.id = PRIMARY_ENCODER,
			.phase_a_get_status = &primary_enc_phase_a_get_status,
			.phase_b_get_status = &primary_enc_phase_b_get_status,
			.timer = TIMER_PRIMARY_ENCODER,
			.delta = 0
		}
	}, 

	.on_value_update = NULL
};


void encoder_init() {
	// Activate internal pull-ups for encoder pins
	PORTC |= (1 << PC0) | (1 << PC1);
	
	timer_register(
}

void encoder_register (void (*on_value_update)(uint8_t)) {
	encoder_ctrl.on_value_update = on_value_update;
}

static void encoder_check() {

}
