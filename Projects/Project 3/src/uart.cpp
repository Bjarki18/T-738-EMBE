#include <avr/io.h>
#include "uart.h"

char TxBuffer[32];
int indx, len, duty_cycle, intr_duty_cycle;;

void Uart::Init_Uart()
{
	// 57600 baudrate
	UBRR0H = 0;
	UBRR0L = 16;
	// UCSR0B = (1<<RXEN0) |(1<<TXEN0);
	UCSR0B = (1<<RXEN0) |(1<<TXEN0)|(1<<RXCIE0);
	UCSR0C = (1<<USBS0) |(3<<UCSZ00);
	indx = len = 0;
}

void Uart::reset_TxBuffer(){
	indx = len = 0;
}

void Uart::UART_transmit_TxBuffer(){
	while (indx < len){
		while(!(UCSR0A & (1<<UDRE0))){;}
		UDR0 = TxBuffer[indx];
		indx++;
	}
}


int Uart::strlen(char s[])
{
	int i=0;
	while ( s[i] != 0 )
	i++;
	return i;
}

void Uart::reverse(char s[])
{
	int i,j;
	char c;

	for (i=0, j=strlen(s)-1; i<j; i++, j--){
	c = s[i];
	s[i] = s[j];
	s[j] = c;
	}
}

void Uart::UART_itoa(int n, char s[])
{
	int i,sign;

	if ( (sign = n ) < 0 )
		n = -n;
	i = 0;
	do {
		s[i++] = n % 10 + '0';
	} while ( ( n /= 10 ) > 0 );
	if (sign < 0 )
	s[i++] = '-';
	s[i] = '\0';
	reverse(s);
}

void Uart::UART_load_string_in_TxBuffer(char s[])
{
	while(s[len] != 0)
	{
		TxBuffer[len] = s[len];
		len++;
	}
}

void Uart::UART_load_charVal_in_TxBuffer(int data)
{
	char temp[32];
	UART_itoa(data,temp);
	UART_load_string_in_TxBuffer(temp);
}

void Uart::send_uart_int(int value){
	UART_load_charVal_in_TxBuffer(value); // load encoder value to transmit
	TxBuffer[len] = '\r';
	TxBuffer[len+1] = '\n';
	len += 2;
	UART_transmit_TxBuffer(); // transmit encoder value over UART
	reset_TxBuffer(); // reset transmit buffer
}


void Uart::send_uart_str(char s[]){
	UART_load_string_in_TxBuffer(s); // load encoder value to transmit
	UART_transmit_TxBuffer(); // transmit encoder value over UART
	reset_TxBuffer(); // reset transmit buffer
}
