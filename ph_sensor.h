#ifndef PH_SENSOR_H
#define PH_SENSOR_H

#include "mraa.hpp"
#include "RTClib.h"
#include "lcd_ui.h"
#include "analog_pin.h"
#include "get_settings.h"
#include <string.h>
#include <unistd.h>
#include <stdio.h>

//change depending on known values of calubration
#define PH_NEUTRAL 			7.00
#define PH_ACIDIC 			4.00
#define PH_BASIC 			9.18

//time needed to calibrate in seconds
#define CALIBRATION_TIME 	60
#define PH_SENSOR_APIN 		3

#define PH_SENSOR_SEL_MODE	-1
#define PH_SENSOR_CAL_BASE	0
#define PH_SENSOR_CAL_ACID	1
#define PH_SENSOR_CAL_NEUT	2
#define PH_SENSOR_READ		3

class ph_sensor
{
private:
	analog_pin* i_pin;

	int neutral_phlevel;
	int acid_phlevel;
	int base_phlevel;
public:

	//default pin = 0
	ph_sensor(int);
	~ph_sensor();

	//calibration functions, we can put this into private if we put all of it in 1 calibrate function
	int calibrate_neutral();
	int calibrate_acid();
	int calibrate_base();

	double get_PHvalue();
};

namespace ph_sensor_func
{
	extern ph_sensor* probe;
	extern lcd* lcd_screen;
	extern DateTime* RTC;

	void press_down(void *);
	void press_select(void *);

	void calibrate_base();
	void calibrate_acid();
	void calibrate_neutral();

	void read();

	const extern int option_size;
	const extern std::string ph_func_names[];

	volatile extern int ph_sensor_option;
	extern int ph_sensor_mode;
	extern int row_line;
	extern void(*ph_sensor_functions[])(void);


	void start(void *);
}

#endif