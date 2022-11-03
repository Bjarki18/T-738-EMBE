#include<iostream>
#include<unistd.h>

int main(){
    int counter = 0;
    static int mult = 2;
    int count_mult = 0;
    while(1){
        std::cin >> counter;
        count_mult = counter*mult;
        std::cout << "Counter before mult: ";
        std::cout << counter;
        std::cout << ", counter multiplied by 2: ";
        std::cout << count_mult << std::endl;
        usleep(1000000); 
    }
}