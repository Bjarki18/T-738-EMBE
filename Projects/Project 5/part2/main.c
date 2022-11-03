#include<stdio.h>
#include<fcntl.h>
#include<unistd.h>
#include<termios.h>
#include<string.h>
#include<stdint.h>
#include<stdlib.h>


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



int main(int argc, char *argv[]){
   int file,count;

    if(argc!=5){
        printf("Invalid number of arguments, exiting!\n");
        return -2;
    }



   if ((file = open("/dev/ttyS0", O_RDWR | O_NOCTTY ))<0){
      perror("UART: Failed to open the file.\n");
      return -1;
   }
    struct termios options;
    tcgetattr(file, &options);
    // raw mode
    cfmakeraw(&options);
    options.c_cflag = B115200 | CS8 | CREAD | CLOCAL;
    options.c_iflag = IGNPAR;
    options.c_cc[VMIN]=8;
    tcflush(file, TCIFLUSH);
    tcsetattr(file, TCSANOW, &options);
    const size_t COMMAND_LEN = 8;
    uint8_t command[COMMAND_LEN];
    uint8_t receive[COMMAND_LEN];

    command[0] = atoi(argv[1]);
    command[1] = atoi(argv[2]);
    command[2] = (atoi(argv[3]) >> 8);
    command[3] = (atoi(argv[3]));
    command[4] = (atoi(argv[4]) >> 8);
    command[5] = (atoi(argv[4]));
    uint16_t crc = ModRTU_CRC(command,6);
    command[6] = (crc >>8);
    command[7] = (crc);


    printf("Sent request: %02hhx",command[0]);
    printf(" %02hhx",command[1]);
    printf(" %02hhx",command[2]);
    printf(" %02hhx",command[3]);
    printf(" %02hhx",command[4]);
    printf(" %02hhx",command[5]);
    printf(" %02hhx",command[6]);
    printf(" %02hhx\n",command[7]);

    // //TESTS
    // printf("test: %04hhx\n",(unsigned char)((command[2]<<8) + command[3]) );
    // printf("test: %04hhx\n",(unsigned char)((command[4]<<8) + command[5]) );

    // write 8 bytes
    if ((count = write(file, command, COMMAND_LEN)<0)){
        perror("Failed to write to the output\n");
        return -1;
    }

    if ((count = read(file, (void*)receive, COMMAND_LEN))<0){
        perror("Failed to read from the input\n");
        return -1;
    }

    // usleep(100000);
    // print the response
    if (count==0) printf("There was no data available to read!\n");
    else {
        receive[count]=0;  //There is no null character sent by the Arduino
        printf("Received reply: ");
        for (int i = 0; i < count; i++){
            printf("%02x ", receive[i]);
        }
        printf("\n");
    }

    close(file);
    return 0;



}



