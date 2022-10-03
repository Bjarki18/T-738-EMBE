#ifndef TIMER1_MS
#define TIMER_MS

class Timer1_ms
{
public:
    Timer1_ms();
    void init(int period_ms);
    void init(int period_ms, float duty_cycle);
    void set(float duty_cycle);
};

#endif 