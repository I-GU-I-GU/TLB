#include"sensor.hpp"

const int proximeter_sensor1_pin = 7;
const int proximeter_sensor2_pin = 5;
const int proximeter_sensor3_pin = 11;
const int proximeter_sensor4_pin = 13;
const int proximeter_sensor5_pin = 15;
const int proximeter_sensor6_pin = 17;

void initial_proximeter_sensor(void)
{
    pinMode(proximeter_sensor1_pin,INPUT);
    pinMode(proximeter_sensor2_pin,INPUT);
    pinMode(proximeter_sensor3_pin,INPUT);
    pinMode(proximeter_sensor4_pin,INPUT);
    pinMode(proximeter_sensor5_pin,INPUT);
    pinMode(proximeter_sensor6_pin,INPUT);
}

bool read_proximeter_sensor(int proximerter_number)
{
    bool sensor_logic = false;
    switch(proximerter_number)
    {
        case 1:
        {
            sensor_logic = digitalRead(proximeter_sensor1_pin);
            break;
        }
        case 2:
        {
            sensor_logic = digitalRead(proximeter_sensor2_pin);
            break;
        }
        case 3:
        {
            sensor_logic = digitalRead(proximeter_sensor3_pin);
            break;
        }
        case 4:
        {
            sensor_logic = digitalRead(proximeter_sensor4_pin);
            break;
        }
        case 5:
        {
            sensor_logic = digitalRead(proximeter_sensor5_pin);
            break;
        }
        case 6:
        {
            sensor_logic = digitalRead(proximeter_sensor6_pin);
            break;
        }
        default:
        {
            // do nothing
        }
    }
    return sensor_logic;
}