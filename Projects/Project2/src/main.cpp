#include <avr/io.h>
#include <avr/interrupt.h>
#include <util/delay.h>
#include <digital_out.h>
#include <digital_in.h>
#include <encoder.h>
#include <timer0_ms.h>
#include <uart.h>
#include <p_controller.h>


Digital_in encoder_c1(DDD2);
Digital_in encoder_c2(DDD3);
Digital_out motor_m1(DDD4);
Digital_out motor_m2(DDD5);
Digital_out pwmPIN(DDD6);
Encoder encoder(10);
P_controller controller(0.01);
Timer0_ms timer0;
Uart uart;
int interval = 1;

ISR(TIMER1_COMPA_vect)
{
	interval += 1;
	encoder.set_speed();
}

ISR(TIMER0_COMPA_vect)
{
  pwmPIN.set_hi();
}


ISR(TIMER0_COMPB_vect)
{
  pwmPIN.set_lo();
}

ISR(INT1_vect)
{
	int b = PIND & (1 << PIND2);
	if (b > 0){
		encoder.increment();
		encoder.speed_increment();
	}else{
		encoder.decrement();
		encoder.speed_decrement();
	}
}

int main(){
	uart.Init_Uart();
	pwmPIN.init();
	timer0.init();
	encoder.timer_msec(10);
	encoder_c1.init();
	encoder_c2.init();
	motor_m1.init();
	motor_m2.init();
	int pwm = 0;
	float target_speed = 600; 
	motor_m1.set_lo();
	motor_m2.set_hi();

	while (1){
		
		OCR0B = controller.update(target_speed, encoder.speed());
		pwm = (1 - (OCR0A - OCR0B)/(OCR0A*1.0)) * 100;
		
		if (interval % 50 == 0){
			uart.send_uart_str("Ref: ");
			uart.send_uart_int(encoder.speed());
			_delay_ms(1);
			uart.send_uart_str("Target: ");
			uart.send_uart_int(target_speed);
			_delay_ms(1);
			uart.send_uart_str("PWM: ");
			uart.send_uart_int(pwm);
			_delay_ms(2);
			interval = 1;
		}else{
			_delay_ms(4);
		}
	}

	return 0;
}
