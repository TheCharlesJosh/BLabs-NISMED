#include "RTClib.h"
#include <cstdio>

int main()
{
	RTC_DS1307::begin();
	if(!RTC_DS1307::isrunning())
	{
		puts("Not running");
		return 0;
	}
	puts("Running");
	
	long next_time = RTC_DS1307::now().unixtime()+1;
	while(1)
	{
		while(RTC_DS1307::now().unixtime()<next_time);
		printf("%ld\n",RTC_DS1307::now().unixtime());
		next_time = RTC_DS1307::now().unixtime()+1;
	}
}
