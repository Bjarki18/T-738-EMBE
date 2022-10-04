#ifndef digital_out
#define digital_out

#include <stdint.h>

class Digital_out {
    public: 
        Digital_out(uint8_t pin_number);
        void init();
        void set_hi();
        void set_lo();
        void toggle();
        void swits(int p);
    private:
        uint8_t pinMask;
        int pin;
};

#endif
