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

#define ENCODER_PORT PINC

#define ENCODER_PHASE_A     (ENCODER_PORT & 1<<PC0)
#define ENCODER_PHASE_B     (ENCODER_PORT & 1<<PC1)

int8_t encode_read1( void );         // read single step encoders
int8_t encode_read2( void );         // read two step encoders
void encoder_init( void );

#endif /* ENCODER_H_ */
