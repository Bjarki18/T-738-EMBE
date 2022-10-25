#include <unistd.h>
#include <time.h>
#include <stdlib.h>
#include <fcntl.h>
#include <stdio.h>

int r = 0;

int main()
{
    int fd = open("/dev/mydev1", O_RDWR);
    srand(time(NULL));
    int counter = 0;
    while(1){
        r = ( rand() % 26 ) + 25;

        
        char *string = malloc(sizeof(char) * (r));
        for (int i = 0; i < r; i++){
            string[i] = 'a';
        } 

        write(fd, &string, r*4);

        usleep(1000000);

        counter++;
        if (counter > 10){
            break;
        }
    }
    close(fd);
    return 0;
}