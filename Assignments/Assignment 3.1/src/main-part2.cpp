#include <Arduino.h>
#include <util/delay.h>

int command = 0; // for incoming serial data
char state = 'r';
int timeout_counter = 0;

void setup()
{
  // put your setup code here, to run once:
  Serial.begin(9600); // opens serial port, sets data rate to 9600 bps
}

void loop()
{
 // put your main code here, to run repeatedly:

  // send data only when you receive data:
  if (Serial.available() > 0)
  {
    // read the incoming byte:
    command = Serial.read(); 
    
    

    // say what you got:
    Serial.print("I received: ");
    Serial.println(command, DEC);
  }
 // you can compare the value received to a character constant, like 'g'.

    _delay_ms(500);
    switch (state)
    {
  
    case 'g':
      if (command == 'g'){;}
      if (command == 's'){state = 'y';}
      Serial.print("Current state: ");
      Serial.println(state);
      break;

    case 'y':
      // if (command == 'g'){;}
      // if (command == 's'){;}
      timeout_counter ++;
      
      if (timeout_counter > 10){
        timeout_counter = 0;
        state='r';
      }

      Serial.print("Current state: ");
      Serial.println(state);
      Serial.println(timeout_counter);
      break;
    

    case 'r':

      if (command == 'g'){state = 'g';}
      if (command == 's'){;}
      Serial.print("Current state: ");
      Serial.println(state);
      break;
    }
  
}