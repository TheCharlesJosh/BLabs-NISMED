#ifndef ANALOG_PIN_H
#define ANALOG_PIN_H

#include "mraa.hpp"

class analog_pin
{
private:
	mraa::Aio *pin;

public:
	analog_pin():pin(0){}
	analog_pin(int pin_num);
	~analog_pin();

	void set_pin(int pin_num);
	int get_value();
};

#endif