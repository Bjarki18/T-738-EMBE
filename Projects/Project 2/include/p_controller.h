
class P_controller {
    public:
        P_controller(double constant);
        double update(double ref, double actual);
        int prev;
    private:
        double K_p;
};

