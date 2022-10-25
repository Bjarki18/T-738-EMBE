#include<stdio.h>
#include<stdint.h>
#include<fcntl.h>
#include<unistd.h>
#include<termios.h> 
#include<pthread.h>
#include<stdlib.h>


uint32_t counter = 0;
int fd, count;

void *writeUART()
{
   while(counter < 15){
      write(fd, &counter, sizeof(counter));
      counter++;
      usleep(1000000);
   }
}

void *readUART()
{
   uint32_t recv;
   while(counter < 15){
      count = read(fd, &recv, sizeof(recv));
      if (count == 0) printf("There was no data available to read!\n");
      else printf("The following was read in [%d]: %d\n",count,recv);
   }
}


int main(){

   if ((fd = open("/dev/ttyS0", O_RDWR | O_NOCTTY))<0){
      perror("UART: Failed to open the file.\n");
      return -1;
   }

   struct termios startOpt;
   tcgetattr(fd, &startOpt);

   struct termios options;
   tcgetattr(fd, &options);

   cfmakeraw(&options);
   options.c_cflag = B115200 | CS8 | CREAD | CLOCAL;
   options.c_cc[VMIN] = sizeof(counter);
   tcflush(fd, TCIFLUSH);
   tcsetattr(fd, TCSANOW, &options);

   pthread_t write;
   pthread_t read;

   pthread_create(&write, NULL, writeUART, NULL);
   pthread_create(&read, NULL, readUART, NULL);
   pthread_join(write, NULL);
   pthread_join(read, NULL);

   //reset opt after run
   tcsetattr(fd, TCSANOW, &startOpt);
   close(fd);

   pthread_exit(NULL);
   return 0;
}