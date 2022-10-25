//Part 2 - 3

#include <unistd.h>
#include <fcntl.h>
#include <poll.h>
#include <stdio.h>

int main()
{
    //Enable gpio20
    int fd = open("/sys/class/gpio/export", O_WRONLY);
    write(fd, "20", 2);
    close(fd);

    //Enable gpio17
    int od = open("/sys/class/gpio/export", O_WRONLY);
    write(od, "17", 2);
    close(od);


    //Set gpio20 as input
    fd = open("/sys/class/gpio/gpio20/direction", O_WRONLY);
    write(fd, "in", 2);
    close(fd);

    //Set gpio17 as output
    od = open("/sys/class/gpio/gpio17/direction", O_WRONLY);
    write(od, "out", 2);
    close(od);


    //Set gpio20 interrupt
    fd = open("/sys/class/gpio/gpio20/edge", O_WRONLY);
    //write(fd, "falling", 7);
    write(fd, "both", 4);
    close(fd);

    

    struct pollfd pfd;
    pfd.fd = fd;
    pfd.events = POLLPRI;

    for(int i=0; i<1000000; i++)
    {
        //Wait for event
        fd = open("/sys/class/gpio/gpio20/value", O_RDONLY);    
        od = open("/sys/class/gpio/gpio17/value", O_WRONLY);    
        int ret = poll(&pfd, 1, 3000);
        char c;
        read(fd, &c, 1);
        close(fd);
        if(ret == 0)
            printf("Timeout\n");
        else
            if(c == '0'){
                printf("push\n");
                write(od, "00", 2);
            }
            else{
                printf("release\n");
                write(od, "01", 2);  
            }
        close(od);
        
    }

    //Disable gpio20
    fd = open("/sys/class/gpio/unexport", O_WRONLY);
    write(fd, "20", 2);
    close(fd);
    //Disable gpio17
    od = open("/sys/class/gpio/unexport", O_WRONLY);
    write(od, "17", 2);
    close(od);

    return(0);
}