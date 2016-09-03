/*
 * Here goes the communication protocol between
 * the uC and the Intel Galileo for the
 * temperature sensor.
 */

#ifndef TEMP_SENSOR_H
#define TEMP_SENSOR_H

#include "mraa.hpp"
#include <string>

#define BAUD_RATE 9600
#define MILLIS_WAIT 100

//#define UART_PARITY_NONE (mraa_uart_parity_t)0

class temp_sensor
{
private:
	mraa::Uart *uart;
public:
	temp_sensor();
	~temp_sensor();

	std::string get_value();
};

#endif