#include "timer_msec.h"
#include <avr/io.h>

Timer_msec::Timer_msec(){
}

void Timer_msec::init(int period_ms){
    // this code sets up timer1 for a 1ms @ 16Mhz Clock (mode 4)
    // Counting 16000/8 cycles of a clock prescaled by 8
    TCCR1A = 0; // set timer1 to normal operation (all bits in control registers A and B set to zero)
    TCCR1B = 0;
    TCNT1 = 0; // initialize counter value to 0
    OCR1A = (period_ms * 16000.0/1024 - 1); // assign target count to compare register A (must be less than 65536)
    TCCR1B |= (1 << WGM12); // clear the timer on compare match A
    TIMSK1 |= (1 << OCIE1A); // set interrupt on compare match A
    TCCR1B |= (1 << CS12) | (1 << CS10); // set prescaler to 1024 and start the timer
}
// To ensure the OCR1A register does not exceed 65536, the period_ms must not exceed
// 65537/(1600/1024) = 4194

void Timer_msec::init(int period_ms, float duty_cycle){
    // this code sets up timer1 for a 1ms @ 16Mhz Clock (mode 4)
    // Counting 16000/8 cycles of a clock prescaled by 8
    TCCR1A = 0; // set timer1 to normal operation (all bits in control registers A and B set to zero)
    TCCR1B = 0;
    TCNT1 = 0; // initialize counter value to 0
    OCR1A = (period_ms * 16000.0/1024 - 1); // assign target count to compare register A (must be less than 65536)
    OCR1B = OCR1A * duty_cycle;
    TCCR1B |= (1 << WGM12); // clear the timer on compare match A
    TIMSK1 |= (1 << OCIE1A) | (1 << OCIE1B); // set interrupt on compare match A & B
    TCCR1B |= (1 << CS12) | (1 << CS10); // set prescaler to 1024 and start the timer
}

// At what timer frequency does the LED appear to change from blinking to a stable intensity?
// at 100ms I can still cleary see blinking 
// at 50ms I can see some blinking
// at 40ms I can barely see some blikning but still see it a little bit
// at 30ms I can't see any blinking and the light appears stable

void Timer_msec::set(float duty_cycle){
    OCR1B = OCR1A * duty_cycle;
}

// How does the intensity of the LED vary when you change the duty cycle?
// When the timer frequency is set to 30ms (when the light is stable) a 
// change in the duty cycle results in the led changing its light intensity
// i.e. the light is brighter at a higher duty cycle, and 
// the light is dim at a lower duty cycle