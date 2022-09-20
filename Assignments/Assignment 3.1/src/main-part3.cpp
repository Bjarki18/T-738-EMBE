#include "states.h"
#include "Arduino.h"
#include "util/delay.h"


Context *context;
char command = 0;

void setup(){
    Serial.begin(9600);
    context = new Context(new Red);
}   

void loop()
{
    command = Serial.read();
    if (command == 'g'){context->go();}
    else if (command == 's'){context->stop();}
    _delay_ms(100);
}