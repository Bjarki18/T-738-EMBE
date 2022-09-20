#ifndef FIFO_H
#define FIFO_H

const int FIFO_SIZE = 5;

class Fifo
{
public:
    Fifo();
    int get();
    void put(int item);
    bool is_empty();
    bool is_full();
    void reset();
    int circular_get();
    void circular_put(int item);
    bool circular_is_empty();
    bool circular_is_full();
    void circular_reset();
private:
    int buffer[FIFO_SIZE];
    int end;
    int counter;
    int* head;
    int* tail;
    int init;
    // add variables pointing to the front and back of the buffer
};

#endif // FIFO_H