#include <fifo.h>
#include <string.h>

Fifo::Fifo()
{
    end = 0;
    counter = 0;
    head = buffer;
    tail = buffer;
    init = 0;
}

int Fifo::get()
{
    int ret_val = buffer[0];
    
    for (int i = 0; i<FIFO_SIZE-1; i++){
        buffer[i] = buffer[i+1];
    }
    buffer[FIFO_SIZE-1] = 0;
    if(end>0){end--;}
    return ret_val;
}

void Fifo::put(int item)
{   
    if (end > FIFO_SIZE-1){get();}
    buffer[end] = item;    
    end++;
}

bool Fifo::is_empty()
{
    return end == 0;
}

bool Fifo::is_full()
{
    return end > (FIFO_SIZE-1);
}

void Fifo::reset()
{
    memset(buffer,0,FIFO_SIZE);
    end = 0;
}


int Fifo::circular_get()
{
    int ret_val = *head; 
    if (counter > 0){
        counter--;
    }
    head++;
    if (head > (int*) (buffer+FIFO_SIZE-1)){
        head = buffer;
    }
    return ret_val;
}

void Fifo::circular_put(int item)
{   
    *tail = item;
    if(counter < 5){
        counter++;
    }
    if (tail == head && init != 0){
        head++;
        if (head > (int*) (buffer+FIFO_SIZE-1)){
            head = buffer;
        }
    }else{
        init = 1;
    }
    tail++;
    if (tail > (int*) (buffer+FIFO_SIZE-1)){
        tail = buffer;
    }
    // tail++;
}    

bool Fifo::circular_is_empty()
{
    return counter == 0;
}

bool Fifo::circular_is_full()
{
    return counter == 5;
}

void Fifo::circular_reset()
{
    memset(buffer,0,FIFO_SIZE);
    counter = 0;
    head = buffer;
    tail = buffer;
    init = 0;
}