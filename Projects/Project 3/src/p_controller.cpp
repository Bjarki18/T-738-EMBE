#include "p_controller.h"
#include <avr/io.h>
#include "digital_out.h"

extern Digital_out motor_m1;
extern Digital_out motor_m2;

P_controller::P_controller(double constant) {
    K_p = constant;
}

double P_controller::update(double ref, double actual) {
    prev = prev + (K_p * ((ref - actual)));
    if (prev > 244){prev = 243;}
    if(prev <1){prev = 1;}
    return prev;
}

void P_controller::update_p(int kp) {
    algo = kp;
}

void P_controller::update_i(int ti) {
    algo = ti;
}

void P_controller::brake(){
    motor_m1.set_lo();
    motor_m2.set_lo();
}

void P_controller::go(){
    motor_m1.set_hi();
    motor_m2.set_lo();
}

void P_controller::alg(int inp){
    algo = inp;
}
