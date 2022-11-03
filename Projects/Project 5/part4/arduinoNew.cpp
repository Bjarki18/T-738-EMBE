#include <Arduino.h>

#define SET_OP 0x01
#define STOP_NODE 0x02
#define SET_PREOP 0x80
#define RESET_NODE 0x81
#define RESET_COMMS 0x82

#define readA bitRead(PIND,2)//faster than digitalRead()
#define readB bitRead(PIND,3)//faster than digitalRead()

const byte encoderPinA = 2;//outputA digital pin2
const byte encoderPinB = 3;//outoutB digital pin3
volatile int count = 0;
int speed_value = 0;



int ledPin = 11;          // LED with PWM brightness control
int PWM = 6; //PWM PIN
int Motor1 = 4;
int Motor2 = 5;
uint8_t server_num = 1;
uint16_t registers[100];
int ThermistorPin = A0;
uint8_t lastState = 0;


void isrA() {
  if(readB != readA) {
    count ++;
  } else {
    count --;
  }
}
void isrB() {
  if (readA == readB) {
    count ++;
  } else {
    count --;
  }
}

void timer_msec(int period_ms){
    // this code sets up timer1 for a 1ms @ 16Mhz Clock (mode 4)
    // Counting 16000/8 cycles of a clock prescaled by 8
    TCCR2A = 0; // set timer1 to normal operation (all bits in control registers A and B set to zero)
    TCCR2B = 0;
    TCNT2 = 0; // initialize counter value to 0
    OCR2A = (period_ms * 16000.0/1024 - 1); // assign target count to compare register A (must be less than 256)
    TCCR2A |= (1 << WGM21); // clear the timer on compare match A
    TIMSK2 |= (1 << OCIE2A); // set interrupt on compare match A
    TCCR2B |= (1 << CS22) | (1 << CS21) | (1 << CS20); // set prescaler to 1024 and start the timer
}


ISR(TIMER2_COMPA_vect)
{
	speed_value = (1000.0*count)/(10*1.0); // pulses
  count = 0;
}


void setup() {            // called once on start up
  // A baud rate of 115200 (8-bit with No parity and 1 stop bit)
  Serial.begin(115200, SERIAL_8N1);
  interrupts();
  pinMode(ledPin, OUTPUT);         // the LED is an output
  pinMode(ThermistorPin,INPUT);
  pinMode(Motor1,OUTPUT);
  pinMode(Motor2,OUTPUT);
  pinMode(encoderPinA, INPUT_PULLUP);
  pinMode(encoderPinB, INPUT_PULLUP);
  pinMode(PWM,OUTPUT);
  timer_msec(10);
  attachInterrupt(digitalPinToInterrupt(encoderPinA), isrA, CHANGE);
  attachInterrupt(digitalPinToInterrupt(encoderPinB), isrB, CHANGE);
}

// Compute the MODBUS RTU CRC
uint16_t ModRTU_CRC(uint8_t buf[], int len)
{
    uint16_t crc = 0xFFFF;
    for (int pos = 0; pos < len; pos++) {
      crc ^= (uint16_t)buf[pos]; // XOR byte into least sig. byte of crc
      for (int i = 8; i != 0; i--) { // Loop over each bit
      if ((crc & 0x0001) != 0) { // If the LSB is set
          crc >>= 1; // Shift right and XOR 0xA001
          crc ^= 0xA001;
      }
      else // Else LSB is not set
          crc >>= 1; // Just shift right
      }
    }
    // Note, this number has low and high bytes swapped, so use it accordingly (or swap bytes)
    return crc;
}

void statemachine(uint16_t state, uint16_t regist){
  switch (state)
  {
    case SET_PREOP:
      if (lastState != SET_PREOP)
      {
        digitalWrite(ledPin,HIGH);
        digitalWrite(Motor1,LOW);
        digitalWrite(Motor2,HIGH);
        lastState = SET_PREOP;
      }

      break;
    case SET_OP:
      if (lastState != SET_OP)
      {
        // TIMSK1 &= ~(1<<OCIE1A);
        analogWrite(PWM,map(registers[regist],0,1023,0,255));
        //controller.go();
        lastState = SET_OP;
      }
      break;
    case STOP_NODE:
      if (lastState != STOP_NODE)
      {
        digitalWrite(Motor1,LOW);
        digitalWrite(Motor2,LOW);
        //controller.brake();
      }
      break;
    case RESET_NODE:
      break;
    case RESET_COMMS:
      break;
  }
}

void loop() {
  // put your main code here, to run repeatedly:
  uint8_t command[8];
  uint8_t buffer[8];
  if (Serial.available()>0){                 // bytes received
    Serial.readBytes(command, 8); // read 2 bytes and place into buffer
    uint8_t server = command[0];
    uint8_t funct = command[1];
    uint16_t regis = ((command[2] << 8) + command[3]);
    uint16_t data = ((command[4] << 8) + command[5]);
    uint16_t crc = ((command[6] << 8) + command[7]);
    
    uint16_t crc_comapre = ModRTU_CRC(command,6);

    //Serial.write(command,8);

    if (crc == crc_comapre){
      if (server_num == server){
        //Read
        if (funct == 3){
            buffer[0] = command[0];
            buffer[1] = command[1];
            buffer[2] = command[2];
            buffer[3] = command[3];
            if(regis == 1){
              uint16_t signal = map(speed_value,0,5600,0,1023);
              uint8_t data1 = signal>>8;
              uint8_t data2 = signal;
              buffer[4] = data1;
              buffer[5] = data2;
              lastState = data;
              registers[regis] = signal;
            }else if(regis == 6){
              uint16_t signal = analogRead(ThermistorPin);
              uint8_t data1 = signal>>8;
              uint8_t data2 = signal;
              buffer[4] = data1;
              buffer[5] = data2;
              lastState = data;
              registers[regis] = signal;
            }
            uint16_t crc_update = ModRTU_CRC(buffer,6);
            buffer[6] = (crc_update >>8);
            buffer[7] = crc_update;
            Serial.write(buffer,8);               // send the buffer to the RPi
        }//Write
        else if (funct == 6){
          if (regis ==0){
            statemachine(data,6);
          }else if(regis == 6){
            registers[regis] = data;
          }else{
            registers[regis] = data;
          }
            lastState = data;
            buffer[0] = command[0];
            buffer[1] = command[1];
            buffer[2] = command[2];
            buffer[3] = command[3];
            buffer[4] = command[4];
            buffer[5] = command[5];
            buffer[6] = command[6];
            buffer[7] = command[7];
            Serial.write(buffer,8);               // send the buffer to the RPi
        }
      }else{
        analogWrite(ledPin, 0);       // yes, write out
      }
    }
  } 
}
