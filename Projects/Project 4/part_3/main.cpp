#include <unistd.h>
#include <fcntl.h>
// #include <poll.h>
#include <stdio.h>
#include <stdlib.h>

// https://pubs.opengroup.org/onlinepubs/000095399/functions/read.html
char c[20];
size_t nbytes;
ssize_t readBytes;

int fd;
float encoder_pulses = 0;
char buff[5];
float error = 0.0;
float the_pwm = 0.0;
float scaler = 100000;
float sum = 0.0;
float value = 0.0;
float integral = 0.0;

//PI controller values
float K_p =  0.001;
float t_i = 2.7;
float t = 0.9;



int update(float ref, float actual) {
    
    error = ref - actual*10;
    value = sum + (error * t);
    if(actual > 1 ){
        sum = value;
    }
    integral = (sum/t_i);
    //PWM range from 00000 up to 99999 (max is 100000), because period set at 100000.
    the_pwm = (K_p * (error + integral)) * scaler;

    if(the_pwm > scaler-1)
    {
        the_pwm = scaler-1;
    }
    if(the_pwm <1)
    {
        the_pwm = 0;
    }
    printf("Required speed in PPS: %f\n", ref);
    printf("Speed in PPS: %f\n", actual*10);  

    return (int)the_pwm;
}


void set_speed(int pwm_val)
{
    fd = open("/sys/class/pwm/pwmchip0/pwm0/duty_cycle", O_WRONLY);
    sprintf(buff, "%d", pwm_val);
    write(fd, buff, sizeof(buff));
    close(fd);
}


void initialize_pwm_and_motor()
{
    fd = open("/sys/class/pwm/pwmchip0/export", O_WRONLY);
    write(fd, "00", 2);
    close(fd);

    fd = open("/sys/class/pwm/pwmchip0/pwm0/period", O_WRONLY);
    write(fd, "100000", 6);
    close(fd);

    fd = open("/sys/class/pwm/pwmchip0/pwm0/duty_cycle", O_WRONLY);
    write(fd, "20000", 5);
    close(fd);

    fd = open("/sys/class/pwm/pwmchip0/pwm0/enable", O_WRONLY);
    write(fd, "01", 2);
    close(fd);

    //Enable gpio26
    fd = open("/sys/class/gpio/export", O_WRONLY);
    write(fd, "26", 2);
    close(fd);

    //Enable gpio16
    fd = open("/sys/class/gpio/export", O_WRONLY);
    write(fd, "16", 2);
    close(fd);

    //Set gpio26 as output
    fd = open("/sys/class/gpio/gpio26/direction", O_WRONLY);
    write(fd, "out", 3);
    close(fd);

    //Set gpio16 as output
    fd = open("/sys/class/gpio/gpio16/direction", O_WRONLY);
    write(fd, "out", 3);
    close(fd);

    fd = open("/sys/class/gpio/gpio26/value", O_WRONLY); 
    write(fd, "00", 2); 
    close(fd);

    fd = open("/sys/class/gpio/gpio16/value", O_WRONLY);  
    write(fd,"01",2);
    close(fd);
}


void unexport()
{
    fd = open("/sys/class/pwm/pwmchip0/unexport", O_WRONLY);
    write(fd, "00", 2);
    close(fd);

    fd = open("/sys/class/gpio/unexport", O_WRONLY);
    write(fd, "26", 2);
    close(fd);

    fd = open("/sys/class/gpio/unexport", O_WRONLY);
    write(fd, "16", 2);
    close(fd);
}


int main()
{
        initialize_pwm_and_motor();
    int ref_speed = 700;
    int previousEncoder = 0;
    int speed = 0;
    int current_pwm = 0;
    int modifier_encounter_val = 0;
    
    for(int i = 0; i < 100000; i++)
    {
        fd = open("/sys/module/encoder_count/parameters/encoderCounter", O_RDONLY); 
        nbytes = sizeof(c);
        readBytes = read(fd, &c, nbytes);
        c[readBytes] = '\0';
        encoder_pulses = atoi(c);
        close(fd);

        if (i == 0)
        {
            modifier_encounter_val = encoder_pulses;
        }
        
        encoder_pulses = encoder_pulses - modifier_encounter_val;
        speed = (encoder_pulses - previousEncoder);

        current_pwm = update(ref_speed, speed);
        set_speed(current_pwm);
        previousEncoder = encoder_pulses;

        usleep(100000);
    }
    unexport();
    return(0);
}
