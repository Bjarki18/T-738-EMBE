#include "states.h"
#include "Arduino.h"


void Green::go(){
    this->context_ -> transition_to(new Green);
}

void Green::stop(){
    this->context_ -> transition_to(new Yellow);
}

void Green::on_entry(){
    Serial.println("Green");
}

void Yellow::on_entry(){
    Serial.println("Yellow");
    _delay_ms(1000);
    this->context_ -> transition_to(new Red);
}

void Red::go(){
    this->context_ -> transition_to(new Green);
}

void Red::stop(){
    this->context_ -> transition_to(new Red);
}

void Red::on_entry(){
    Serial.println("Red");
}
