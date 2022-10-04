#include "p_controller.h"
#include <avr/io.h>


P_controller::P_controller(double constant) {
    K_p = constant;
}

double P_controller::update(double ref, double actual) {
    prev = prev + (K_p * (ref - actual));
    if (prev > 244){prev = 243;}
    if(prev <1){prev = 1;}
    return prev;
}


