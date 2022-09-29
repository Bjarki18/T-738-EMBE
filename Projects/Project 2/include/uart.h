class Uart {
    public: 
        void Init_Uart();
        void reset_TxBuffer();
        void UART_transmit_TxBuffer();
        int strlen(char s[]);
        void reverse(char s[]);
        void UART_itoa(int n, char s[]);
        void UART_load_string_in_TxBuffer(char s[]);
        void UART_load_charVal_in_TxBuffer(int data);
        void send_uart_int(int value);
        void send_uart_str(char s[]);
};