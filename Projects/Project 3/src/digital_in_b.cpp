#include "Digital_in_b.h"
#include <avr/io.h>

Digital_in_b::Digital_in_b(uint8_t pin_number){
    pinMask = (1<<pin_number);
}

void Digital_in_b::init(){
    DDRB &= ~pinMask;
    PORTB |= pinMask;
}

bool Digital_in_b::is_hi(){
    return PINB & pinMask;
}

bool Digital_in_b::is_lo(){
    return !(PINB & pinMask);
}
