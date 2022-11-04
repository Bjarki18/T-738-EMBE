#include<stdio.h>
#include<fcntl.h>
#include<unistd.h>
#include<termios.h>
#include<string.h>
#include<stdint.h>
#include<stdlib.h>

#define COMMAND_LEN 8

// int file,count;
uint8_t command[COMMAND_LEN];
uint8_t receive[COMMAND_LEN];
uint16_t lastServo0 = 0;
uint16_t lastServo1 = 0;








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

void Read( uint8_t servernum){
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


int main(int argc, char *argv[]){

    // if(argc!=5){
    //     printf("Invalid number of arguments, exiting!\n");
    //     return -2;
    // }
    int file,count;
    uint16_t crc;
    // const size_t COMMAND_LEN = 8;
    // uint8_t command[COMMAND_LEN];
    // uint8_t receive[COMMAND_LEN];
    
    if ((file = open("/dev/ttyS0", O_RDWR | O_NOCTTY ))<0){
        perror("UART: Failed to open the file.\n");
        return -1;
    }


    Init(file);

    Preop(0x00);
    SentReq();
    Send(file);

    Receive(file);

    Preop(0x01);
    SentReq();
    Send(file);
    Receive(file);

    for(int i = 0; i < 1000; i++){

        Read(0x00);
        SentReq();
        Send(file);
        Receive(file);

        Write(0x01,0x06,receive[4],receive[5]);
        SentReq();
        Send(file);
        Receive(file);

        Op(0x01);
        SentReq();
        Send(file);
        Receive(file);
        
        Read(0x01);
        SentReq();
        Send(file);
        Receive(file);


        Write(0x00,0x06,receive[4],receive[5]);
        SentReq();
        Send(file);
        Receive(file);


        Op(0x00);
        SentReq();
        Send(file);
        Receive(file);
        usleep(10000);
    }
    Stop(0x00);
    SentReq();
    Send(file);
    Receive(file);

    Stop(0x01);
    SentReq();
    Send(file);
    Receive(file);
    close(file);
    return 0;
}
