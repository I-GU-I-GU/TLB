void reset_io_control(void);
void run_silo_roller(int roller_number);
void initial_io_control(void);
void stop_silo_roller(void);

void open_tray(void);
void close_tray(void);
void turnoff_actuator(void);
void Relay_ON(void);
void Relay_OFF(void);


bool read_over_limit(void);
bool read_limit_switch(void);
void release_printer(void);
void operate_printer(void);
void reset_printer_operation(void);

int get_conveyor_status(void);
void on_conveyor(void);
void reset_conveyor(void);
void check_conveyor(void);
void reinit_conveyor(void);


void clear_lamp(void);
void on_lamp(char lamp_number);

void on_motor_flip_forward(void);
void on_motor_flip_backward(void);
void off_motor_flip(void);
void linere_forward(void);
void linere_backward(void);
void off_linere(void);
bool check_flip_tube(void);