#include<iostream>
#include<unistd.h>

int main(){
    int counter = 0;
    while(1){
        std::cout << counter << std::endl;
        counter++;
        usleep(1000000); 
    }
}