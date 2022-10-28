#include <Arduino.h>

int ledPin = 11;          // LED with PWM brightness control
uint8_t server_num = 1;

void setup() {            // called once on start up
  // A baud rate of 115200 (8-bit with No parity and 1 stop bit)
  Serial.begin(115200, SERIAL_8N1);
  pinMode(ledPin, OUTPUT);         // the LED is an output
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

void loop() {
  // put your main code here, to run repeatedly:
  uint8_t command[8];
  uint8_t buffer[8];
  if (Serial.available()>0){                 // bytes received
    Serial.readBytes(command, 8); // read 2 bytes and place into buffer
    // buffer[0] = command[0];
    // buffer[1] = command[1];
    // buffer[2] = command[2];
    // buffer[3] = command[3];
    // buffer[4] = command[4];
    // buffer[5] = command[5];
    // buffer[6] = command[6];
    // buffer[7] = command[7];
    uint8_t server = command[0];
    uint8_t funct = command[1];
    uint16_t regis = ((command[2] << 8) + command[3]);
    uint16_t data = ((command[4] << 8) + command[5]);
    uint16_t crc = ((command[6] << 8) + command[7]);
    

    if (server_num == server){
      analogWrite(ledPin, 255);       // yes, write out
      Serial.write(command,8);               // send the buffer to the RPi
    }else{
      analogWrite(ledPin, 0);       // yes, write out
    }
  } 
}