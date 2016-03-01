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

#define ENCODER_PORT		PORTD
#define ENCODER_PORT_IN 	PIND

#define ENCODER_PIN_PHASE_A PD2
#define ENCODER_PIN_PHASE_B PD3
#define ENCODER_PHASE_A     (ENCODER_PORT_IN & 1<<ENCODER_PIN_PHASE_A)
#define ENCODER_PHASE_B     (ENCODER_PORT_IN & 1<<ENCODER_PIN_PHASE_B)

int8_t encode_read1( void );         // read single step encoders
int8_t encode_read2( void );         // read two step encoders
int8_t encode_read4( void );         // read four step encoders
void encoder_init( void );

#endif /* ENCODER_H_ */
