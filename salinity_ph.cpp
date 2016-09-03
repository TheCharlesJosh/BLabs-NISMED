#include "cond_probe.h"
#include "ph_sensor.h"
#include "RTClib.h"
#include <cstdio>

#define SALINITY_INTERV 15
#define PH_INTERV 15
#define UNION_INTERV 15

ph_sensor phs;

void log_salinity()
{
	long next_time = RTC_DS1307::now().unixtime();
	while(1)
	{
		if(RTC_DS1307::now().unixtime()>=next_time)
		{
			next_time += SALINITY_INTERV;
			printf("%.2lf\n",cond_probe::read());
		}
	}
}

void log_ph()
{
	long next_time = RTC_DS1307::now().unixtime();
	while(1)
	{
		if(RTC_DS1307::now().unixtime()>=next_time)
		{
			next_time += PH_INTERV;
			printf("%.2lf\n",phs.get_PHvalue());
		}
	}
}

void log()
{
	long next_time = RTC_DS1307::now().unixtime();
	while(1)
	{
		if(RTC_DS1307::now().unixtime()>=next_time)
		{
			next_time += UNION_INTERV;
			printf("Salinity: %.2lf%%\tpH: %.2lf\n",cond_probe::read(),phs.getPHvalue());
		}
	}
}

void log_ph

void init()
{
	RTC_DS1307::begin();
	cond_probe::begin();
	phs = ph_sensor(0);
}

int main()
{
	init();
	log();
}