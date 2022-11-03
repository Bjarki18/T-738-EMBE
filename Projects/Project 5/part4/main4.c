#include<stdio.h>
#include<fcntl.h>
#include<unistd.h>
#include<termios.h>
#include<string.h>
#include<stdint.h>
#include<stdlib.h>


#define COMMAND_LEN 8
uint8_t command[COMMAND_LEN];
uint8_t receive[COMMAND_LEN];
float error = 0.0;
float the_pwm = 0.0;
float scaler = 100;
float value = 0.0;
float integral = 0.0;
float sumM0 = 0;
float sumM1 = 0;

//PI controller values
// float K_p =  0.0001;
// float t_i = 2.7;
// float t = 0.09;

float K_p =  0.0001;
float t_i = 0.1;
float t = 0.09;

// float K_p =  2.7;
// float t_i = 0.009;
// float t = 0.009;

long map(long x, long in_min, long in_max, long out_min, long out_max)
{
  return (x - in_min) * (out_max - out_min) / (in_max - in_min) + out_min;
}

uint16_t PI_controllerM0(float ref, float actual) {
    
    error = map(ref,0,1023,0,5600) - map(actual,0,1023,0,5600) ;
    value = sumM0 + (error * t);
    if(actual > 1 ){
        sumM0 = value;
    }
    integral = (sumM0/t_i);
    //PWM range from 0 up to 99 (max is 100)
    the_pwm = (K_p * (error + integral))*100;

    if(the_pwm > scaler-1)
    {
        the_pwm = scaler-1;
    }
    if(the_pwm <1)
    {
        the_pwm = 0;
    }
    printf("Required speed in PPS: %f\n", ref);
    printf("Speed in PPS: %f\n", actual);  
    
    return (uint16_t)map(the_pwm,0,100,0,1023);
}


uint16_t PI_controllerM1(float ref, float actual) {
    
    error = map(ref,0,1023,0,5600) - map(actual,0,1023,0,5600) ;
    value = sumM1 + (error * t);
    if(actual > 1 ){
        sumM1 = value;
    }
    integral = (sumM1/t_i);
    //PWM range from 0 up to 99 (max is 100)
    the_pwm = (K_p * (error + integral))*100;

    if(the_pwm > scaler-1)
    {
        the_pwm = scaler-1;
    }
    if(the_pwm <1)
    {
        the_pwm = 0;
    }
    printf("Required speed in PPS: %f\n", ref);
    printf("Speed in PPS: %f\n", actual);  
    
    return (uint16_t)map(the_pwm,0,100,0,1023);
}

// Compute the MODBUS RTU CRC
uint16_t ModRTU_CRC(uint8_t buf[], int len)
{
    uint16_t crc = 0xFFFF;
        for (int pos = 0; pos < len; pos++) {
        crc ^= (uint16_t)buf[pos]; // XOR byte into least sig. byte of crc
        for (int i = 8; i != 0; i--) { // Loop over each bit
        if ((crc & 0x0001) != 0) { // If the LSB is set
            crc >>= 1; // Shift right and XOR 0xA001
            crc ^= 0xA001;
        }
        else // Else LSB is not set
            crc >>= 1; // Just shift right
        }
    }
    // Note, this number has low and high bytes swapped, so use it accordingly (or swap bytes)
    return crc;
}

void Preop(uint8_t servernum){
    command[0] = servernum;
    command[1] = 0x06;
    command[2] = 0x00;
    command[3] = 0x00;
    command[4] = 0x00;
    command[5] = 0x80;
    uint16_t crc = ModRTU_CRC(command,6);
    command[6] = (crc >>8);
    command[7] = (crc);
    // Send();
}

void Op(uint8_t servernum){
    command[0] = servernum;
    command[1] = 0x06;
    command[2] = 0x00;
    command[3] = 0x00;
    command[4] = 0x00;
    command[5] = 0x01;
    uint16_t crc = ModRTU_CRC(command,6);
    command[6] = (crc >>8);
    command[7] = (crc);
    // Send();
}

void Stop(uint8_t servernum){
    command[0] = servernum;
    command[1] = 0x06;
    command[2] = 0x00;
    command[3] = 0x00;
    command[4] = 0x00;
    command[5] = 0x02;
    uint16_t crc = ModRTU_CRC(command,6);
    command[6] = (crc >>8);
    command[7] = (crc);
    // Send();
}

void ReadPot( uint8_t servernum){
    command[0] = servernum;
    command[1] = 0x03;
    command[2] = 0x00;
    command[3] = 0x06;
    command[4] = 0x00;
    command[5] = 0x01;
    uint16_t crc = ModRTU_CRC(command,6);
    command[6] = (crc >>8);
    command[7] = (crc);
    // Send();
}

void ReadSpeed( uint8_t servernum){
    command[0] = servernum;
    command[1] = 0x03;
    command[2] = 0x00;
    command[3] = 0x01;
    command[4] = 0x00;
    command[5] = 0x01;
    uint16_t crc = ModRTU_CRC(command,6);
    command[6] = (crc >>8);
    command[7] = (crc);
    // Send();
}

void Write(uint8_t servernum, uint16_t regisnumb, uint8_t data1, uint8_t data2){
    command[0] = servernum;
    command[1] = 0x06;
    command[2] = regisnumb >> 8;
    command[3] = regisnumb;
    command[4] = data1;
    command[5] = data2;
    uint16_t crc = ModRTU_CRC(command,6);
    command[6] = (crc >>8);
    command[7] = (crc);
}

void Init(int file){
    struct termios options;
    tcgetattr(file, &options);
    // raw mode
    cfmakeraw(&options);
    options.c_cflag = B115200 | CS8 | CREAD | CLOCAL;
    options.c_iflag = IGNPAR;
    options.c_cc[VMIN]=8;
    tcflush(file, TCIFLUSH);
    tcsetattr(file, TCSANOW, &options);
}

void SentReq(){
    printf("Sent request:");
    for (int i = 0; i < 8; i++){
        printf("%02x ", command[i]);
    }
    printf("\n");
}

int Send(int file){
    int count;
    // write 8 bytes
    if ((count = write(file, command, 8)<0)){
        perror("Failed to write to the output\n");
        return -1;
    }
}

void Receive(int file){
    int count;
    // printf("FOKKING HATE THIS SHIT \n: ");
    if ((count = read(file, (void*)receive, COMMAND_LEN))<0){
        perror("Failed to read from the input\n");
    }

        // print the response
    if (count==0) printf("There was no data available to read!\n");
    else {
        receive[count]='\0';  //There is no null character sent by the Arduino
        printf("Received reply: ");
        for (int i = 0; i < count; i++){
            printf("%02x ", receive[i]);
        }
        
        printf("\n");

    }
}


int main(int argc, char *argv[]){

    // if(argc!=5){
    //     printf("Invalid number of arguments, exiting!\n");
    //     return -2;
    // }
    int file,count;
    // FILE *csvFile;
    uint16_t crc;
    uint16_t servoPOT;
    uint16_t servoAcutalSpeed;
    uint16_t pwm;
    int ref = 0;
    // const size_t COMMAND_LEN = 8;
    // uint8_t command[COMMAND_LEN];
    // uint8_t receive[COMMAND_LEN];
    
    if ((file = open("/dev/ttyS0", O_RDWR | O_NOCTTY ))<0){
        perror("UART: Failed to open the file.\n");
        return -1;
    }

    // csvFile = fopen("Motor1Load.csv","w+");
    // fprintf(csvFile,"ref, act, pwm\n");
    Init(file);

    Preop(0x00);
    SentReq();
    Send(file);
    Receive(file);

    Preop(0x01);
    SentReq();
    Send(file);
    Receive(file);

    for(int i = 0; i < 100; i++){

        ReadPot(0x00);
        SentReq();
        Send(file);
        printf("POT0:\n");
        Receive(file);
        servoPOT = ((receive[4] << 8) + receive[5]);

        ReadSpeed(0x01);
        SentReq();
        Send(file);
        Receive(file);
        servoAcutalSpeed = ((receive[4] << 8) + receive[5]);
        pwm = PI_controllerM1(servoPOT,servoAcutalSpeed);

        //   if (i < 250){
        //     pwm = PI_controllerM1(ref,servoAcutalSpeed);
        // }else{
        //     ref = 800;
        //     pwm = PI_controllerM1(ref,servoAcutalSpeed);
        // }

        // pwm = PI_controllerM1(800,servoAcutalSpeed);
        // fprintf(csvFile,"%d, %d, %d\n",ref,servoAcutalSpeed,pwm);



        Write(0x01,0x06,(pwm>>8),pwm);
        SentReq();
        Send(file);
        Receive(file);

        Op(0x01);
        SentReq();
        Send(file);
        Receive(file);
        
        ReadPot(0x01);
        SentReq();
        Send(file);
        printf("POT1:\n");
        Receive(file);
        servoPOT = ((receive[4] << 8) + receive[5]);

        ReadSpeed(0x00);
        SentReq();
        Send(file);
        Receive(file);
        servoAcutalSpeed = ((receive[4] << 8) + receive[5]);
        pwm = PI_controllerM0(servoPOT,servoAcutalSpeed);
        
        
        printf("PWM: %d\n",pwm);
        printf("SUMM!: %f\n",sumM0);



        //   if (i < 250){
        //     pwm = PI_controllerM0(ref,servoAcutalSpeed);
        // }else{
        //     ref = 800;
        //     pwm = PI_controllerM0(ref,servoAcutalSpeed);
        // }
        // pwm = PI_controllerM0(800,servoAcutalSpeed);

        // fprintf(csvFile,"%d, %d, %d\n",ref,servoAcutalSpeed,pwm);




        Write(0x00,0x06,(pwm>>8),pwm);
        SentReq();
        Send(file);
        Receive(file);

        Op(0x00);
        SentReq();
        Send(file);
        Receive(file);

        usleep(10000);
    }
    //
    Stop(0x00);
    SentReq();
    Send(file);
    Receive(file);

    Stop(0x01);
    SentReq();
    Send(file);
    Receive(file);

    // fclose(csvFile);
    close(file);
    return 0;
}



