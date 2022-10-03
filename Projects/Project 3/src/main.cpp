#include <avr/io.h>
#include <avr/interrupt.h>
#include <util/delay.h>
#include <digital_out.h>
#include <digital_in.h>
#include <encoder.h>
#include <timer0_ms.h>
#include <uart.h>
#include <p_controller.h>
#include <timer1_ms.h>

Digital_in encoder_c1(DDD2);
Digital_in encoder_c2(DDD3);
Digital_out motor_m1(DDD4);
Digital_out motor_m2(DDD5);
Digital_out pwmPIN(DDD6);
Digital_out LED(DDD7); //Led
Encoder encoder(10);
P_controller controller(0.01);
Timer0_ms timer0;
Timer1_ms timer1;
Uart uart;

int interval = 1;
char state = 'i';
char last_state = 'r';
char command = '';

// ISR(TIMER1_COMPA_vect)
// {
// 	interval += 1;
// 	encoder.set_speed();
// }

// ISR(TIMER0_COMPA_vect)
// {
//   pwmPIN.set_hi();
// }

// ISR(TIMER0_COMPB_vect)
// {
//   LED.toggle();
// }

ISR(TIMER1_COMPA_vect)
{
  // action to be done on x ms time x depending on input into init function
  LED.toggle();

}

ISR (USART_RX_vect)
{
	command = UDR0;
	if (state == 'i'){
		;
	}else if (state == 'p' && (command == 'i' || command == 'o')){
    	state = command;
	}else if (state == 'o' && (command == 'p' || command == 'i')){
		state = command;
	}else if (state == 's'){
		state = command;
	}
}

// ISR(INT1_vect)
// {
// 	int b = PIND & (1 << PIND2);
// 	if (b > 0){
// 		encoder.increment();
// 		encoder.speed_increment();
// 	}else{
// 		encoder.decrement();
// 		encoder.speed_decrement();
// 	}
// }


int main(){
	sei();
	while(1){
		uart.send_uart_int(state);
		_delay_ms(100);
		// OCR0B = controller.update(target_speed, encoder.speed());
		// pwm = (1 - (OCR0A - OCR0B)/(OCR0A*1.0)) * 100;
		
		// if (interval % 50 == 0){
		// 	uart.send_uart_str("Ref: ");
		// 	uart.send_uart_int(encoder.speed());
		// 	_delay_ms(1);
		// 	uart.send_uart_str("Target: ");
		// 	uart.send_uart_int(target_speed);
		// 	_delay_ms(1);
		// 	uart.send_uart_str("PWM: ");
		// 	uart.send_uart_int(pwm);
		// 	_delay_ms(2);
		// 	interval = 1;
		// }else{
		// 	_delay_ms(4);
		// }
		// if (Serial.available() > 0)
		// {
		// 	// read the incoming byte:
		// 	state = Serial.read();
		// 	// say what you got:
		// 	Serial.print("I received: ");
		// 	Serial.println(state, DEC);
		// }
		
		switch (state)
		{
			case 'i':
		
				// Serial.println("Initialization State");
				uart.Init_Uart();
				// pwmPIN.init();
				// timer0.init();
				LED.init();
				encoder.timer_msec(10);
				encoder_c1.init();
				encoder_c2.init();
				motor_m1.init();
				motor_m2.init();
				// int pwm = 0;
				// float target_speed = 600; 
				// uart.send_uart_str("Init state");
				state = 'p';
				break;
			case 'o':
				// uart.send_uart_str("Oper state");
				// Serial.println("Operational State");
				if (state != last_state){
					last_state = state;
					TIMSK1 &= ~(1<<OCIE1A);
					LED.set_hi();
				}
				
				break;	

			case 'p':
				if (last_state != state){
					timer1.init(1000);
					last_state = state;
				}
				// uart.send_uart_str("Pre-oper state");
				
				// OCR1A = (16000.0 * 1000)/1024 - 1;
				break;

			case 's':
				if (last_state != state){
					timer1.init(500);
					last_state = state;
				}
		}
		// while (!(UCSR0A & (1 << RXC0))){;}
		// state = UDR0;
	}
}
