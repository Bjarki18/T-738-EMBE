#include <digital_out.h>
#include <timer_msec.h>

#include <avr/interrupt.h>

Digital_out led(5);
Timer_msec timer;

float duty = 0.2;

int main()
{
  led.init();
  timer.init(1000,0.2);
  sei(); // enable interrupts
  
  while(1)
  {
  }
}

ISR(TIMER1_COMPA_vect)
{
  // action to be taken at the start of the on-cycle
  // changing duty cycle between 10% and 90% every on-cycle interrupt
  if (duty < 1.0){
    timer.set(duty);
    duty += 0.1;
  }else{
    duty = 0.1;
    timer.set(duty);
  }
  led.toggle();
}

ISR (TIMER1_COMPB_vect)
{
  // action to be taken at the start of the off-cycle
  led.toggle();
}