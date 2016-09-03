#include "cond_probe.h"
#include "RTClib.h"

int main()
{
	cond_probe::begin();
	RTC_DS1307::begin();
	while(1)
	{
		RTC_DS1307::RTC_sleep(3);
		printf("%.3lf\n",cond_probe::read());
	}
}