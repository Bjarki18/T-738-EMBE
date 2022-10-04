class Encoder {
    public: 
        Encoder(int speed_time);
        int position();
        void increment();
        void speed_increment();
        void decrement();
        void speed_decrement();
        void timer_msec(int var);
        float speed();
        void set_speed();
    private:
        int pos;
        float speed_value;
        int speed_counter;
        int speed_time;
};