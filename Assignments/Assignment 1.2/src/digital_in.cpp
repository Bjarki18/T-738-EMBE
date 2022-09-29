#include "Digital_in.h"
#include <avr/io.h>

Digital_in::Digital_in(uint8_t pin_number){
    pinMask = pin_number;
}

void Digital_in::init(){
    DDRB |= 0<<pinMask;
}

bool Digital_in::is_hi(){
    return PINB & (1<< pinMask);
}

bool Digital_in::is_lo(){
    return !(PINB & (1 << pinMask));
}
