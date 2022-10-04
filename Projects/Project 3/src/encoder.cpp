#include "encoder.h"
#include <avr/io.h>


Encoder::Encoder(int speed_tim){
    EIMSK |= (1<<INT1); // external interrupt mask register for INT1 interrupt vector
	EICRA |= (1<<ISC10) | (1<<ISC11); // falling + rising edge interrupt requests
    asm("sei"); // enable interrupts
    speed_time = (float) speed_tim;
}

void Encoder::increment(){
    pos += 1;
}

void Encoder::decrement(){
    pos -= 1;
}

void Encoder::speed_increment(){
    speed_counter += 1;
}

void Encoder::speed_decrement(){
    speed_counter -= 1;
}

int Encoder::position(){
    return pos;
}

float Encoder::speed(){
    return speed_value;
}

void Encoder::set_speed(){
    // speed_value= (((speed_counter*1000.0/ speed_time*1.0)) / 1400.0) * 60.0; // revolutions
    speed_value = (1000.0*speed_counter)/(speed_time*1.0); // pulses
    speed_counter = 0;
}

void Encoder::timer_msec(int period_ms){
    // this code sets up timer1 for a 1ms @ 16Mhz Clock (mode 4)
    // Counting 16000/8 cycles of a clock prescaled by 8
    TCCR2A = 0; // set timer1 to normal operation (all bits in control registers A and B set to zero)
    TCCR2B = 0;
    TCNT2 = 0; // initialize counter value to 0
    OCR2A = (period_ms * 16000.0/1024 - 1); // assign target count to compare register A (must be less than 256)
    TCCR2A |= (1 << WGM21); // clear the timer on compare match A
    TIMSK2 |= (1 << OCIE2A); // set interrupt on compare match A
    TCCR2B |= (1 << CS22) | (1 << CS21) | (1 << CS20); // set prescaler to 1024 and start the timer
}