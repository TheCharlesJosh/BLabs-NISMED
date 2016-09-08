/*
 * Here goes the communication protocol between
 * the uC and the Intel Galileo for the
 * temperature sensor.
 */

#ifndef TEMP_SENSOR_H
#define TEMP_SENSOR_H

#include "mraa.hpp"
#include <string>
#include "lcd_ui.h"
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <stdio.h>

#define BAUD_RATE 9600
#define MILLIS_WAIT 100

#define READ_INTERVAL 5		//minimum of 10
#define THRESHOLD 0.5

//#define UART_PARITY_NONE (mraa_uart_parity_t)0

#define TEMP_SENSOR_MODE_EQUIL	0
#define TEMP_SENSOR_MODE_READ	1


class temp_sensor
{
private:
	mraa::Uart *uart;
public:
	temp_sensor();
	~temp_sensor();

	std::string get_value();
};

namespace temp_sensor_func
{
	extern temp_sensor* probe1;
	extern lcd* lcd_screen;
	extern DateTime* RTC;

	void press_down(void *);
	void press_select(void *);

	void equilibrium();
	void read();

	const extern int option_size;
	const extern std::string temp_func_names[];

	volatile extern int temp_sensor_option;
	extern int temp_sensor_mode;
	extern int row_line;
	extern void(*temp_sensor_functions[])(void);


	void start(void *);
}

#endif