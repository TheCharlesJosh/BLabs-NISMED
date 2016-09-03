#include "mraa.hpp"
#include "temp_sensor.h"
#include "RTClib.h"
#include <cstdio>
#include <utility>
#include <string>

#define READ_INTERVAL 5		//minimum of 10

typedef std::pair<double,double> dd;

//intervals in array form
int interv[] = {15,15};
temp_sensor ts;

int main()
{
	RTC_DS1307::begin();
	puts("CLOCK INITIALIZED");

	long next_time = RTC_DS1307::now().unixtime()+READ_INTERVAL;

	while(1)
	{
		if(RTC_DS1307::now().unixtime()>=next_time)
		{
			next_time += READ_INTERVAL;

			std::string str = ts.get_value();

			printf("%s",str.substr(0,str.find('\n')+1).c_str());
		}
	}
}