#include<sensors.hpp>
//************************************************//
const int prox1_pin = 3;
const int prox2_pin = 5;
const int prox3_pin = 7;
const int prox4_pin = 9;
// const int prox5_pin = 13;
// const int prox6_pin = 15;

const int infrared_sensor = 19;

const int light_sensor = 46; //อ่านหลอดในSilo//
//***********************************************//
void init_sensors(void){
    // proximeter sensor pin
    pinMode(prox1_pin,INPUT);
    pinMode(prox2_pin,INPUT);
    pinMode(prox3_pin,INPUT);
    pinMode(prox4_pin,INPUT);
    // pinMode(prox5_pin,INPUT);
    // pinMode(prox6_pin,INPUT);
    // infrared sensor
    pinMode(infrared_sensor,INPUT);
    /////////////////////////////
    pinMode(light_sensor,INPUT);
    /////////////////////////////
}

//**********************************************//
bool check_tube_drop(void){
    return digitalRead(infrared_sensor);  
}

bool Read_tube(void){
    return digitalRead(light_sensor);
}



//*********************************************//
void reset_sensor_variables(void){
    
}

// *********************************************//
// return silo number
int get_proximeter_values(void){
    byte sensor_value = 0x00;
    // sensor_value = (sensor_value<<1) + digitalRead(prox6_pin);
    // sensor_value = (sensor_value<<1) + digitalRead(prox5_pin);
    sensor_value = (sensor_value<<1) + digitalRead(prox4_pin);
    sensor_value = (sensor_value<<1) + digitalRead(prox3_pin);
    sensor_value = (sensor_value<<1) + digitalRead(prox2_pin);
    sensor_value = (sensor_value<<1) + digitalRead(prox1_pin);
    sensor_value = sensor_value & 0x0F;
    int return_position = 0;
    switch(sensor_value){
        case 0x0E:
        {
            return_position = 1;
            break;
        }
        case 0x0D:
        {
            return_position = 2;
            break;
        }
        case 0x0B:
        {
            return_position = 3;
            break;
        }
        case 0x07:
        {
            return_position = 4;
            break;
        }
        default:
        {
            return_position = int(sensor_value);
        }
    }
    return return_position;
}