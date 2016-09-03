#ifndef COND_PROBE
#define COND_PROBE

#include "cond_probe.h"
#include "RTClib.h"
#include <cstdio>
#include <csignal>

int cond_probe::get_response(mraa::I2c *i2c, uint8_t data[32])
{
	i2c->read(data,32);
	if(data[0]==NO_DATA)
	{
		puts("Error in cond_probe::read() - NO_DATA");
		return -1;
	}
	if(data[0]==FAILED)	
	{
		puts("Error in cond_probe::read() - FAILED");
		return -1;
	}
	return 0;
}

void cond_probe::send_command(mraa::I2c *i2c, const char command[])
{
	uint8_t data[32];
	strcpy((char *)data,command);
	i2c->write(data,strlen((char *)data));
}

void cond_probe::begin()
{
	mraa::I2c *i2c = new mraa::I2c(0);
	i2c->address(COND_PROBE_ADDRESS);

	uint8_t data[32];
	send_command(i2c,"O,EC,0");
	usleep(300000);
	if(get_response(i2c,data))
		return;

	send_command(i2c,"O,TDS,0");
	usleep(300000);
	if(get_response(i2c,data))
		return;

	send_command(i2c,"O,S,1");
	usleep(300000);
	if(get_response(i2c,data))
		return;

	send_command(i2c,"O,SG,0");
	usleep(300000);
	if(get_response(i2c,data))
		return;

	send_command(i2c,"K,1.0");
	usleep(300000);
	if(get_response(i2c,data))
		return;
}

void cond_probe::calibrate(int type)
{
	mraa::I2c *i2c = new mraa::I2c(0);
	i2c->address(COND_PROBE_ADDRESS);
	uint8_t data[32];

	send_command(i2c,"Cal,clear");
	usleep(300000);
	if(get_response(i2c,data))
		return;

	long d;
	scanf("%ld",&d);

	send_command(i2c,"Cal,dry");
	RTC_DS1307::RTC_sleep(2);
	if(get_response(i2c,data))
		return;

	if(type==1)
	{
		scanf("%ld",&d);
		sprintf((char *)data,"Cal,one,%ld",d);
		send_command(i2c,(const char *)data);
		RTC_DS1307::RTC_sleep(2);
		if(get_response(i2c,data))
			return;
	}
	else if(type==2)
	{
		scanf("%ld",&d);
		sprintf((char *)data,"Cal,low,%ld",d);
		send_command(i2c,(const char *)data);
		RTC_DS1307::RTC_sleep(2);
		if(get_response(i2c,data))
			return;

		scanf("%ld",&d);
		sprintf((char *)data,"Cal,high,%ld",d);
		send_command(i2c,(const char *)data);
		RTC_DS1307::RTC_sleep(2);
		if(get_response(i2c,data))
			return;
	}
}

double cond_probe::read()
{
	mraa::I2c *i2c = new mraa::I2c(0);
	i2c->address(COND_PROBE_ADDRESS);
	send_command(i2c,"R");
	RTC_DS1307::RTC_sleep(1);

	uint8_t data[32];
	if(get_response(i2c,data))
		return -1.00;

	return atof((const char *)data+1);
}

#endif