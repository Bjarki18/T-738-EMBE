#include <util/delay.h>
#include <digital_in.h>
#include <digital_out.h>

int main()
{
  Digital_in button(0);   
  Digital_out led(5);
  button.init();
  led.init();

  while (1)
  {
    _delay_ms(1000);
    if(button.is_hi()){
      led.set_lo();
    }else{
      led.toggle();
    }
  }

  return 0;
}