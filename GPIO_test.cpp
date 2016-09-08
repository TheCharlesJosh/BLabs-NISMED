#include "mraa.hpp"
#include "./lib/ph_sensor.h"
#include <unistd.h>
#include <iostream>
#include "grove.h"

int main()
{
	mraa::Gpio *LED = new mraa::Gpio(7);
	mraa::Gpio *Button = new mraa::Gpio(3);

	if(LED == NULL || Button == NULL)
		return 1;

	LED->dir(mraa::DIR_OUT);
	Button->dir(mraa::DIR_IN);

	int count = 10, cur_LED = 0;
	LED->write(cur_LED);
	while(count != 0)
	{
		int temp = Button->read();
		if(temp == 1)
		{
			std::cout << temp << std::endl;
			LED->write(cur_LED ^= 1);
			count--;
		}
		sleep(1);
	}

	delete LED;
	delete Button;
}