#include "analog_pin.h"
#include "mraa.hpp"

~analog_pin()
{
	if(pin)
		delete pin;
	pin = 0;
}

analog_pin::analog_pin(int pin_num)
{
	pin = new mraa::Aio(pin_num);
}

void analog_pin::set_pin(int pin_num)
{
	if(pin)
	{
		delete pin;
	}
	pin = new mraa::Aio(pin_num);
}

int analog_pin::get_value()
{
	return pin->read();
}