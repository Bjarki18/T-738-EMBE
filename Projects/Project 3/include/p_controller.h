
class P_controller {
    public:
        P_controller(double constant);
        double update(double ref, double actual);
        int prev;
        void brake();
        void go();
        void alg(int inp);
        void update_p(int kp);
        void update_i(int ti);
    private:
        double K_p;
        double T_i;
        int algo;
};

