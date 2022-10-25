#include <Arduino.h>

/* Arduino Serial Command Processor */

int ledPin = 11;          // LED with PWM brightness control

void setup() {            // called once on start up
   // A baud rate of 115200 (8-bit with No parity and 1 stop bit)
   Serial.begin(115200, SERIAL_8N1);
   pinMode(ledPin, OUTPUT);         // the LED is an output
}

void loop() {              // loops forever
   // String command;
   char buffer[100];       // stores the return buffer on each loop   
   if (Serial.available()>0){                 // bytes received
      Serial.readBytes(buffer, 2);
      // command = Serial.readStringUntil('\0'); // C strings end with \0
      if(buffer[0] == 2){   // first part with 2?
      
         // String intString = command.substring(4, command.length());
         uint8_t level = buffer[1];       // extract the int
         if(level>=0 && level<=255){          // is it in range?
            analogWrite(ledPin, level);       // yes, write out
            sprintf(buffer, "Set brightness to %d\n", level);
         }
         else{                                // no, error message back
            sprintf(buffer, "Error: %d is out of range\n", level);
         } 
      }                                       // otherwise, unknown cmd
      else{ sprintf(buffer, "Unknown command: %d\n", (int) buffer[0]); }
      Serial.print(buffer);               // send the buffer to the RPi
   }
}