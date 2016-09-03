#include "ph_sensor.h"
#include "analog_pin.h"

ph_sensor::ph_sensor(int apin)
{
	RTC_DS1307::begin();
	i_pin = analog_pin(apin);
	neutral_phlevel = 511;
	acid_phlevel = 0;
	base_phlevel = 1023;
}

ph_sensor::~ph_sensor()
{
}

int ph_sensor::calibrate_neutral()
{
	RTC_DS1307::RTC_sleep(1);

	int total_PH = 0;
	for(int i = 0; i < CALIBRATION_TIME; i++)
	{
		total_PH += i_pin.get_value();
		RTC_DS1307::RTC_sleep(1);
	}

	return this->neutral_phlevel = total_PH/CALIBRATION_TIME;
}

int ph_sensor::calibrate_acid()
{
	RTC_DS1307::RTC_sleep(1);

	int total_PH = 0;
	for(int i = 0; i < CALIBRATION_TIME; i++)
	{
		total_PH += i_pin.get_value();
		RTC_DS1307::RTC_sleep(1);
	}

	return this->acid_phlevel = total_PH/CALIBRATION_TIME;
}

int ph_sensor::calibrate_base()
{
	RTC_DS1307::RTC_sleep(1);

	int total_PH = 0;
	for(int i = 0; i < CALIBRATION_TIME; i++)
	{
		total_PH += i_pin.get_value();
		RTC_DS1307::RTC_sleep(1);
	}

	return base_phlevel = total_PH/CALIBRATION_TIME;
}

double ph_sensor::get_PHvalue()
{
	int temp = i_pin.get_value();

	if(temp < this->neutral_phlevel)
	{
		//calculate ph for acid
		double slope = (this->neutral_phlevel - this->acid_phlevel)/(PH_NEUTRAL - PH_ACIDIC);
		return PH_NEUTRAL - slope*(temp - this->neutral_phlevel);
	}
	else
	{
		//calculate ph for basic
		double slope = (this->base_phlevel - this->neutral_phlevel)/(PH_BASIC - PH_NEUTRAL);
		return slope*(temp - this->neutral_phlevel) + PH_NEUTRAL;
	}
}