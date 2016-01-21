/************************************************************************/
/*                                                                      */
/*                      Reading rotary encoder                      */
/*                      one, two and four step encoders supported   */
/*                                                                      */
/*              Author: Peter Dannegger                                 */
/*                                                                      */
/************************************************************************/
#include "encoder.h"
 
 
volatile int8_t enc_delta;          // -128 ... 127
static int8_t last;
 
 
void encoder_init( void )
{
	int8_t new;
	
 	// Activate internal pull-ups for encoder pins
	PORTC |= (1 << PC0) | (1 << PC1);
	
	new = 0;
	if( ENCODER_PHASE_A ) {
		new = 3;
	}
	if( ENCODER_PHASE_B ) {
		new ^= 1;             // convert gray to binary
	}
	last = new;                   // power on state
	enc_delta = 0;



	//init encoder timer interrupt
	//prescaler
	TCCR0 = (1 << CS01); // | (1 << CS00);
	//enable interrupt
	TIMSK |= (1 << TOIE0);
}
 
 
ISR( TIMER0_OVF_vect )
{
  int8_t new = 0, diff = 0;
 
  if( ENCODER_PHASE_A )
    new = 3;
  if( ENCODER_PHASE_B )
    new ^= 1;                   // convert gray to binary
  diff = last - new;                // difference last - new
  if( diff & 1 ){               // bit 0 = value (1)
    last = new;                 // store new as next last
    enc_delta += (diff & 2) - 1;        // bit 1 = direction (+/-)
  }
}
 
 
int8_t encode_read1( void )         // read single step encoders
{
	int8_t val;
 
	ATOMIC_BLOCK(ATOMIC_RESTORESTATE) {
		val = enc_delta;
		enc_delta = 0;
	}

	return val;                   // counts since last call
}

int8_t encode_read2( void )         // read two step encoders
{
  int8_t val;
 
  cli();
  val = enc_delta;
  enc_delta = val & 1;
  sei();
  return val >> 1;
}
