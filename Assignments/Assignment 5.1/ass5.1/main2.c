#include<stdio.h>
#include<fcntl.h>
#include<unistd.h>
#include<termios.h>   // using the termios.h library

int main(){
   int fd, count;

   // Remove O_NDELAY to *wait* on serial read (blocking read)
   if ((fd= open("/dev/ttyS0", O_RDWR | O_NOCTTY | O_NDELAY))<0){
      perror("UART: Failed to open the file.\n");
      return -1;
   }

   struct termios options;       // the termios structure is vital
   tcgetattr(fd, &options);    // sets the parameters for the file

   // Set up the communication options:
   // 115200 baud, 8-N-1, enable receiver, no modem control lines
   options.c_cflag = B57600 | CS8 | CREAD | CLOCAL;
   options.c_iflag = IGNPAR | ICRNL;   // ignore partity errors
   tcflush(fd, TCIFLUSH);            // discard file information
   tcsetattr(fd, TCSANOW, &options); // changes occur immmediately


   unsigned char receive[100]; //declare a buffer for receiving data
   unsigned char transmit[100]; //declare a buffer for transmitting data

   while(1){
      // input
      printf("User input: ");
      scanf(" %[^\n]", &transmit);
      
      // write
      if ((count = write(fd, &transmit, 100))<0){
         perror("Failed to write to the output\n");
         return -1;
      }

      usleep(100000);

      // read
      if ((count = read(fd, (void*)receive, 100))<0){
         perror("Failed to read from the input\n");
         return -1;
      }
      if (count==0) printf("There was no data available to read!\n");
      else printf("The following was read in [%d]: %s\n",count,receive);

   }


   close(fd);

   return 0;
}