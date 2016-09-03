#ifndef TEMP_SENSOR
#define TEMP_SENSOR

#include "mraa.hpp"
#include "temp_sensor.h"
#include <cstdio>
#include "RTClib.h"

temp_sensor::temp_sensor()
{
	uart = new mraa::Uart(0);
	uart->setBaudRate(BAUD_RATE);
	uart->setMode(8,mraa::UART_PARITY_NONE,1);
	uart->setFlowcontrol(false,false);
}

temp_sensor::~temp_sensor()
{
	if(uart)
		delete uart;
}

std::string temp_sensor::get_value()
{
	std::string str = "1";
	uart->writeStr(str);
	uart->flush();

	if(uart->dataAvailable(1500))
	{
		str = "";
		while(uart->dataAvailable(MILLIS_WAIT))
			str += uart->readStr(1);

		//printf("->%lf %s\n",ret,str.c_str());
		return str;
	}
	return "";
}

#endif