#include <avr/io.h>
#include <util/delay.h>

int main()
{
  DDRB |= 1 << PB5; // initalize led pin
  while(1){
    PORTB ^= 1 << PB5; //Toggle LED
    _delay_ms(500);
    // OR
    // PORTB |= 1 << PB5; //Turn LED ON
    //_delay_ms(500);
    // PORTB &= 1 << PB5; //Turn LED OFF
  }
}