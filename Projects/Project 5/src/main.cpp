#include <avr/io.h>
#include <avr/interrupt.h>

char TxBuffer[32], cData;
int indx, len;
// c1,c2,c3 coefficients from https://www.circuitbasics.com/arduino-thermistor-temperature-sensor-tutorial/
float c1 = 1.009249522e-03, c2 = 2.378405444e-04, c3 = 2.019202697e-07;
float R, logR, temp;

void Init_Uart()
{
	// 57600 baudrate
	UBRR0H=0;
	UBRR0L=16;
	UCSR0B = (1<<RXEN0) |(1<<TXEN0);// |(1<<TXCIE0);
	UCSR0C=(1<<USBS0) |(3<<UCSZ00);
	indx = len = 0;
}

void reset_TxBuffer(){
	indx = len = 0;
}


void UART_transmit_TxBuffer(){
	while (indx < len){
		while(!(UCSR0A & (1<<UDRE0))){;}
		UDR0 = TxBuffer[indx];
		indx++;
	}
}

int strlen(unsigned char s[])
{
	int i=0;
	while ( s[i] != 0 )
	i++;
	return i;
}

//https://www.geeksforgeeks.org/convert-floating-point-number-string/
void reverse(char* str, int len)
{
    int i = 0, j = len - 1, temp;
    while (i < j) {
        temp = str[i];
        str[i] = str[j];
        str[j] = temp;
        i++;
        j--;
    }
}


//https://www.geeksforgeeks.org/convert-floating-point-number-string/
int intToStr(int x, char str[], int d)
{
    int i = 0;
    while (x) {
        str[i++] = (x % 10) + '0';
        x = x / 10;
    }

    // If number of digits required is more, then
    // add 0s at the beginning
    while (i < d)
        str[i++] = '0';

    reverse(str, i);
    str[i] = '\0';
    return i;
}

//https://www.geeksforgeeks.org/convert-floating-point-number-string/
void ftoa(float n, char res[], int nOfDecimals){
	// Extract integer part
	int ipart = (int)n;

	// Extract floating part
	float fpart = n - (float)ipart;

	// convert integer part to string
	int i = intToStr(ipart, res, 0);

	// check for display option after point
	if (nOfDecimals != 0) {
		res[i] = '.'; // add dot

		// Get the value of fraction part up to given no.
		// of points after dot. The third parameter
		// is needed to handle cases like 233.007
		double x = 1;

		for (int i = nOfDecimals; i>0;i--){
			x = x * 10;
		}
		fpart = fpart * x;

		intToStr((int)fpart, res + i + 1, nOfDecimals);
	}
}

void UART_load_string_in_TxBuffer(char s[])
{
	while(s[len] != 0)
	{
		TxBuffer[len] = s[len];
		len++;
	}
}

void UART_load_charVal_in_TxBuffer(float data)
{
	char temp[32];
	ftoa(data,temp,2);
	UART_load_string_in_TxBuffer(temp);
}


void checkReady(){
	
	while ( ! ( UCSR0A & (1<<RXC0) ) ){;}
	cData= UDR0;
}


//https://www.geeksforgeeks.org/program-to-compute-log-n/
unsigned int msb(unsigned int n)
{
    return (n > 1) ? 1 + msb(n / 2) : 0;
}

//https://stackoverflow.com/questions/9799041/efficient-implementation-of-natural-logarithm-ln-and-exponentiation
float ln(float y) {
    int log2;
    float divisor, x, result;

    log2 = msb((int)y); // See: https://stackoverflow.com/a/4970859/6630230
    divisor = (float)(1 << log2);
    x = y / divisor;    // normalized value between [1.0, 2.0]

    result = -1.7417939 + (2.8212026 + (-1.4699568 + (0.44717955 - 0.056570851 * x) * x) * x) * x;
    result += ((float)log2) * 0.69314718; // ln(2) = 0.69314718

    return result;
}



int main()
{
	//choose vref as 5V and single ended ADC0 input (presume 0x00after reset):
	ADMUX = (1 << REFS0);
	//ADCSRB untouched as it is default in free=running=mode after reset
	//power on ADC, activate free running mode, start converting , 128 prescaler (=>125kHz)
	ADCSRA |= (1<<ADEN)|(1 << ADATE)|(1<<ADSC)|(1<<ADPS2)|(1<<ADPS1) |(1<<ADPS0);

	while (1) {
		checkReady();
		UART_load_string_in_TxBuffer("Temp in C: ");
		UART_transmit_TxBuffer();
		reset_TxBuffer();


		// Rearranged Voltage divider equation (Rtherm = Ro * (Vin / Vout - 1))
		// Standard equation is Vout = Vin * (Ro / (Ro + Rtherm) )
		R = 10000 * (1023.0 / ((float) ADCW) - 1.0);

		// Steinhart-Hart thermistor equation
		logR = ln(R);
		temp = ((1.0 / (c1 + c2*logR + c3*logR*logR*logR)) - 273.15);

		UART_load_charVal_in_TxBuffer(temp);
    TxBuffer[len] = '\r';
    TxBuffer[len+1] = '\n';
    len += 2;
		UART_transmit_TxBuffer();
		reset_TxBuffer();
	}
	return 0 ;
}