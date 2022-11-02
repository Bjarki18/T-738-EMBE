#include <Arduino.h>
#include "digital_out.h"
#include <digital_in_b.h>
#include <digital_in.h>
#include <encoder.h>
#include <timer0_ms.h>

#define SET_OP 0x01
#define STOP_NODE 0x02
#define SET_PREOP 0x80
#define RESET_NODE 0x81
#define RESET_COMMS 0x82

Digital_in encoder_c1(DDD2);
Digital_in encoder_c2(DDD3);
Digital_out motor_m1(DDD4);
Digital_out motor_m2(DDD5);
Digital_out pwmPIN(DDD6);
Encoder encoder(10);
// PI_controller controller(1.0, 0.064);
Timer0_ms timer0;
pwmPIN.init();
timer0.init();
encoder.timer_msec(10);
encoder_c1.init();
encoder_c2.init();
motor_m1.init();
motor_m2.init();

volatile int posi = 0;

int ledPin = 11;          // LED with PWM brightness control
uint8_t server_num = 0;
uint16_t registers[100];
int ThermistorPin = A0;
uint8_t lastState = 0;
void setup() {            // called once on start up
  // A baud rate of 115200 (8-bit with No parity and 1 stop bit)
  Serial.begin(115200, SERIAL_8N1);
  pinMode(ledPin, OUTPUT);         // the LED is an output
  pinMode(ThermistorPin,INPUT);
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
       lastState = SET_PREOP
      }
      break;
    case SET_OP:
      if (lastState != SET_OP)
      {
        TIMSK1 &= ~(1<<OCIE1A);
        //controller.go();
        lastState = SET_OP
      }
      break;
    case STOP_NODE:
      if (lastState != STOP_NODE)
      {
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

    if (crc == crc_comapre){
      if (server_num == server){
        //Read
        if (funct == 3){
            buffer[0] = command[0];
            buffer[1] = command[1];
            buffer[2] = command[2];
            buffer[3] = command[3];
            if (regis == 0){
              //statemachine(data,regis);
              uint16_t signal = analogRead(ThermistorPin);
              lastState = data;
              uint8_t data1 = signal>>8;
              uint8_t data2 = signal;
              buffer[4] = data1;
              buffer[5] = data2;
              registers[regis] = signal;
            }else{
              uint8_t data1 = registers[regis]>>8;
              uint8_t data2 = registers[regis];
              buffer[4] = data1;
              buffer[5] = data2;
            }
            uint16_t crc_update = ModRTU_CRC(buffer,6);
            buffer[6] = (crc_update >>8);
            buffer[7] = crc_update;
            Serial.write(buffer,8);               // send the buffer to the RPi
        }//Write
        else if (funct == 6){
            statemachine(data,regis);
            lastState = data;
            buffer[1] = command[1];
            buffer[2] = command[2];
            buffer[3] = command[3];
            buffer[4] = command[4];
            buffer[5] = command[5];
            buffer[6] = command[6];
            buffer[7] = command[7];
            registers[regis] = data;
            Serial.write(buffer,8);               // send the buffer to the RPi
        }
      }else{
        analogWrite(ledPin, 0);       // yes, write out
      }
    }
  } 
}
