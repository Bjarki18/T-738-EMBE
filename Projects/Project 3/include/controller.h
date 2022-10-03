#ifndef controller_h
#define controller_h

#include <avr/io.h>
#include "digital_out.h"

extern Digital_out motor_m1;
extern Digital_out motor_m2;

int abs(int val);

class Controller
{
public:
    // P_controller(double constant);
    virtual double update(double ref, double actual) = 0;
    virtual void brake() = 0;
    virtual void go() = 0;
    virtual ~Controller() {}

    // private:
    //     double k_p;
    //     double k_i;
};

class P_controller : public Controller
{
public:
    int prev;
    P_controller(double constant) { K_p = constant; }
    double update(double ref, double actual) { 
        prev = prev + (K_p * (ref - actual));
        if (prev > 244){prev = 243;}
        if(prev <1){prev = 1;}
        OCR0B = prev;
        return prev;
    }
    void brake(){
        motor_m1.set_lo();
        motor_m2.set_lo();
    }
    void go(){
        motor_m1.set_hi();
        motor_m2.set_lo();
    }

private:
    double K_p;
};
class PI_controller : public Controller
{
public:
    double pi = 0;
    PI_controller(double p, double i)
    {
        sum = 0;
        k_p = p;
        t_i = i;
    }
    double update(double ref, double actual)
    {
        double error = ref - actual;
        sum += error * t;
        

        pi = k_p * (error  + sum/t_i);
        
        // if (pi > 0)
        // {
        //     motor_m1.set_hi();
        //     motor_m2.set_lo();
        // }
        // else
        // {
        //     motor_m1.set_lo();
        //     motor_m2.set_hi();
        // }

        if (pi > 244)
        {
            pi = 243;
            sum -= error * t;
        }
        if (pi < -244)
        {
            pi = -243;
            sum -= error * t;
        }


        OCR0B = abs(pi);
        return pi;
    }
    void brake(){
        motor_m1.set_lo();
        motor_m2.set_lo();
    }
    void go(){
        motor_m1.set_hi();
        motor_m2.set_lo();
    }
    void update_p(int kp){
        k_p = kp * 0.01;
    }
    void update_i(int ti){        
        t_i = 4 + (3.5 - 1.5) * (ti- 5)/(5 - 1);
    }

private:
    double k_p;
    double t_i;
    double sum = 0;
    double t = 0.006;
};


int abs(int val)
{
    if (val < 0)
    {
        return -1 * val;
    }
    else
    {
        return val;
    }
}


#endif
