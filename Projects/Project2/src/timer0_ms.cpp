#include "timer0_ms.h"
#include <avr/io.h>

Timer0_ms::Timer0_ms()
{
    comp = 244;
}

void Timer0_ms::init()
{
    TCCR0A = 0;
    TCCR0B = 0;
    TCNT0 = 0;
    OCR0A = comp;
    OCR0B = 0;
    TCCR0A |= (1 << WGM01);
    TIMSK0 |= (1 << OCIE0A) | (1 << OCIE0B);
    TCCR0B |= (0 << CS02) | (0 << CS01) | (1 << CS00);
}
