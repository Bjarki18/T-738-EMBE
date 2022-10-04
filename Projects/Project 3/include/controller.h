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
    virtual double update(double ref, double actual);
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
        if (prev >= 244){prev = 243;}
        if(prev <1){prev = 1;}
        OCR0B = prev;
        return prev;
    }
    void brake(){
        motor_m1.set_hi();
        motor_m2.set_hi();
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
        
        //in order to find Kp
        // pi = k_p * (error  + (sum/t_i)*0);
        
        // if (pi > 0 && last != 1)
        // {
        //     motor_m1.set_hi();
        //     motor_m2.set_lo();
        //     last = 1;
        // }
        // else if (pi <= 0 && last !=0)
        // {
        //     motor_m1.set_lo();
        //     motor_m2.set_hi();
        //     last = 0;
        // }

        if (pi >= 244.0)
        {
            pi = 243.0;
            sum -= error * t;
        }
        else if (pi <= -244.0)
        {
            pi = -243.0;
            sum -= error * t;
        }

        OCR0B = abs(floor(pi));
        return pi;
    }
    void brake(){
        motor_m1.set_hi();
        motor_m2.set_hi();
    }
    void go(){
        motor_m1.set_hi();
        motor_m2.set_lo();
    }
    void update_p(int kp){
        // k_p = kp * 0.01;
        k_p = kp * 1.0;
    }
    void update_i(int ti){        
        // t_i = 4 + (3.5 - 1.5) * (ti- 5)/(5 - 1);
        t_i = ti * 0.015;
    }

private:
    double pi = 0;
    double k_p = 0.0;
    double t_i = 0.0;
    double sum = 0;
    double t = 0.001;
    // double t = 0.075;
    int last = 5;
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
