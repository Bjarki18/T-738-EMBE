#include <avr/io.h>
#include <avr/interrupt.h>
#include <util/delay.h>
#include "digital_out.h"
#include <digital_in_b.h>
#include <digital_in.h>
#include <encoder.h>
#include <timer0_ms.h>
#include <uart.h>
// #include <p_controller.h>
#include <timer1_ms.h>
#include "controller.h"


// piController.control(u);
#define STOP 1

Digital_in encoder_c1(DDD2);
Digital_in encoder_c2(DDD3);
Digital_out motor_m1(DDD4);
Digital_out motor_m2(DDD5);
Digital_out pwmPIN(DDD6);
Digital_out LED(DDD7);
Digital_in_b btn_stop(DDB0);
Encoder encoder(10);
// P_controller controller(0.01);
PI_controller controller(1.0, 0.064);
// PI_controller controller(2.7, 0.064);
Timer0_ms timer0;
Timer1_ms timer1;
Uart uart;
int interval = 1;
int command = 0; 
char state = 'i';
volatile uint8_t button_stop = 0;
char last_state = 'r';
// int pwm = 0;
float target_speed = 600; 
// int button_check = 1;
int y = 0;


ISR(TIMER2_COMPA_vect)
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


ISR(TIMER1_COMPA_vect)
{
  // action to be done on x ms time x depending on input into init function
  LED.toggle();

}

ISR (USART_RX_vect)
{
	command = UDR0;

	if (state == 'i'){
		state = 'i';
	}else if (state == 'p' && (command == 'i' || command == 'o' || (command >= 48 && command <= 57))){
		if((command >= 48 && command <= 52)){ // 0-4 numpad
			controller.update_p(command - 47);	// scale 0-4 ascii to 1-5 int
		}
		else if (command >= 53 && command <= 57){ // 5-9 numpad
			controller.update_i(command - (49+5)); //scale 5-9 ascii (53-57) down to 1-5 int
			
		}else{
    		state = command;
		}
	}else if (state == 'o' && (command == 'p' || command == 'i')){
		state = command;
	}else if (state == 's'){
		state = command;
	}

	if (command == 'x'){
		P_controller controller(0.01);
	}

	if (command == 'c'){
		PI_controller controller(0.01,2.5);
	}
}




void debounce()
{
	// Counter for number of equal states
	static uint8_t count = 0;
	uint8_t button = 0;
	// Keeps track of current (debounced) state
	static uint8_t button_state = 0;

	uint8_t current_state = 0;
	// Check which button is high or low for the moment

	if (btn_stop.is_lo()){
		current_state = 1;
		button = STOP;
	}

	if (current_state != button_state) {
		// Button state is about to be changed, increase counter
		count++;
		if (count >= 4) {
			// The button have not bounced for four checks, change state
			button_state = current_state;

			// If the button was pressed (not released), tell main so
			if (current_state != 0) {
				if (button == STOP){
					button_stop = 1;
				}
			}
			count = 0;
		}
	} else {
		// Reset counter
		count = 0;
	}
}


int main(){
	sei();	
	while(1){

		debounce();

		if(button_stop){
			button_stop = 0;
			state = 's';
		}
		// _delay_ms(1);
		int pwm = (1 - (OCR0A - OCR0B)/(OCR0A*1.0)) * 100;
		if (interval % 50 == 0){
			uart.send_uart_str("Ref: ");
			uart.send_uart_int(encoder.speed());
			_delay_ms(1);
			uart.send_uart_str("Target: ");
			uart.send_uart_int(target_speed);
			_delay_ms(1);
			uart.send_uart_str("PWM: ");
			uart.send_uart_int(pwm);
			_delay_ms(1);
			uart.send_uart_str("upd: ");
			uart.send_uart_int(y);
			_delay_ms(1);
			interval = 1;
		}else{
			_delay_ms(4);
		}


		switch (state)
		{
			case 'i':
				uart.Init_Uart();
				if (last_state != state){
					uart.send_uart_str("Initialization State");
				}
				btn_stop.init();
				pwmPIN.init();
				timer0.init();
				LED.init();
				encoder.timer_msec(10);
				encoder_c1.init();
				encoder_c2.init();
				motor_m1.init();
				motor_m2.init();
				controller.brake();
				last_state = state;
				state = 'p';
				break;
			case 'p':
				if (last_state != state){
					uart.send_uart_str("Pre-op state");
					timer1.init(1000);
					last_state = state;
					controller.brake();
				}			
				break;
			case 'o':
				if (state != last_state){
					uart.send_uart_str("Op state");
					last_state = state;
					TIMSK1 &= ~(1<<OCIE1A);
					LED.set_hi();
					controller.go();
				}
				y = controller.update(target_speed, encoder.speed());
				// uart.send_uart_int(y);
				// OCR0B = 244;
				// int pwm = (1 - (OCR0A - OCR0B)/(OCR0A*1.0)) * 100;
				// // uart.send_uart_int(encoder.speed());
				// uart.send_uart_int(pwm);
				break;	
			case 's':
				if (last_state != state){
					uart.send_uart_str("stop state");
					timer1.init(500);
					last_state = state;
					controller.brake();
				}
		}
	}
	return 0;
}