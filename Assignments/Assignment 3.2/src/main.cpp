#include <Arduino.h>
#include <fifo.h>
#include <timer_msec.h>
#include <digital_out.h>

int x;
Fifo f;
Digital_out outputPin(DDD2);
Timer_msec timer;

void setup() {
  Serial.begin(9600);
  timer.init(1000);
  outputPin.init();
  sei();
}

void loop() {
  if (Serial.available()){
    x = Serial.read();
    f.put(x);
  }
  if (f.is_full()){
    outputPin.set_hi();
  }else{
    outputPin.set_lo();
  }
}

ISR(TIMER1_COMPA_vect)
{
  Serial.println(f.get());
}