#ifndef PH_SENSOR_H
#define PH_SENSOR_H

#include "mraa.h"
#include "RTClib.h"
#include "analog_pin.h"

//change depending on known values of calubration
#define PH_NEUTRAL 7.00
#define PH_ACIDIC 4.00
#define PH_BASIC 9.18

//time needed to calibrate in seconds
#define CALIBRATION_TIME 60

class ph_sensor
{
private:
	analog_pin i_pin;

	int neutral_phlevel;
	int acid_phlevel;
	int base_phlevel;
public:
	ph_sensor(){};
	ph_sensor(int);
	~ph_sensor();

	//calibration functions, we can put this into private if we put all of it in 1 calibrate function
	int calibrate_neutral();
	int calibrate_acid();
	int calibrate_base();

	double get_PHvalue();
};
#endif