#include "digital_out.h"
#include <avr/io.h>

Digital_out::Digital_out(uint8_t pin_number){
    Digital_out::pinMask = 1<<pin_number;
}

void Digital_out::init(){
    
    DDRD |= Digital_out::pinMask; // DDRB
    
}

void Digital_out::set_hi(){
    
    PORTD |= Digital_out::pinMask; // PORTB
    
}

void Digital_out::set_lo(){
    
    PORTD &= ~(Digital_out::pinMask); // PORTB
    
}

void Digital_out::toggle(){
    PORTD ^= Digital_out::pinMask; // PORTB
}