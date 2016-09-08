#ifndef TEMP_SENSOR
#define TEMP_SENSOR

#include "mraa.hpp"
#include "temp_sensor.h"
#include "csv_export.h"
#include <cstdio>
#include <iomanip>
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
	return "DC\n";
}

namespace temp_sensor_func
{
	temp_sensor* probe = NULL;
	lcd* lcd_screen = NULL;
	DateTime* RTC = NULL;

	const int option_size = 3;
	const std::string temp_func_names[] = {"Therm. Equil.", "Temp. Read", "Return"};
	volatile int temp_sensor_option = 0;
	int temp_sensor_mode = -1;
	int row_line = 0;
	void(*temp_sensor_functions[option_size])(void) = {equilibrium, read, NULL};

	void press_down(void *args)
	{
		if(lcd_screen->check_pressed_down())
			return;

		if(temp_sensor_mode != -1)
			return;

		temp_sensor_option++;
		if(temp_sensor_option >= option_size)
			temp_sensor_option = 0;

		lcd_screen->update_screen(temp_func_names[temp_sensor_option], ((temp_sensor_option == option_size - 1)?(""):(temp_func_names[temp_sensor_option + 1])));
		std::cout << "Pressed temp_sensor_down" << std::endl;
	}

	void press_select(void *args)
	{
		if(lcd_screen->check_pressed_select())
			return;

		if(temp_sensor_mode >= 0)
			temp_sensor_mode = -1;
		else
			temp_sensor_mode = temp_sensor_option;

		std::cout << "Pressed temp_sensor_select" << std::endl;
	}

	void equilibrium()
	{
		lcd_screen->update_screen("Please Wait", "Initializing...");
		std::cout << "READING" << std::endl;
		DateTime dt = RTC_DS1307::now();

		std::vector<std::string> ve;
		ve.push_back(std::string("Date/Time"));
		ve.push_back(std::string("Temp A"));
		ve.push_back(std::string("Temp B"));
		csv_export ce(std::string("temp_equil.csv"),ve.size());

		ce.create_headers_v(ve,',');
		long next_time = RTC_DS1307::now().unixtime()+READ_INTERVAL;

		printf("Initialized\n");
		int row_num = 0;
		while(temp_sensor_mode == TEMP_SENSOR_MODE_EQUIL)
		{
			if(RTC_DS1307::now().unixtime()>=next_time)
			{
				row_num++;
				next_time += READ_INTERVAL;

				std::string str = probe->get_value();
				str = str.substr(0,str.find('\n'));

				dt = RTC_DS1307::now();
				ve[0] = dt.time_stamp();

				char buffer[17] = {};
				snprintf(buffer, 16, "%4d|", row_num);
				std::string row_line(buffer);

				lcd_screen->update_screen("Row#|Temp1,Temp2", row_line + str);

				double a,b;
				sscanf(str.c_str(),"%lf,%lf",&a,&b);
				
				{
					std::stringstream ss;
					ss << std::fixed << std::setprecision(2) << a;
					ve[1] = ss.str();
				}
				{
					std::stringstream ss;
					ss << std::fixed << std::setprecision(2) << b;
					ve[2] = ss.str();
				}
				ce.export_values_v(ve,',');
				
				printf("%s %.2lf %.2lf\n",ve[0].c_str(),a,b);
				if(fabs(a-b)<=THRESHOLD)
				{
					long seconds = (row_num - 1)*READ_INTERVAL;
					snprintf(buffer, 16, "%10ld", seconds);

					lcd_screen->update_screen("Equilibrium", "After " + std::string(buffer) +"s");
					break;
				}
			}
		}
		printf("Done\n");
	}

	void read()
	{
		lcd_screen->update_screen("Please Wait", "Initializing...");
		std::cout << "READING" << std::endl;
		DateTime dt = RTC_DS1307::now();

		std::vector<std::string> ve;
		ve.push_back(std::string("Date/Time"));
		ve.push_back(std::string("Temp A"));
		ve.push_back(std::string("Temp B"));
		csv_export ce(std::string("temp_log.csv"),ve.size());

		ce.create_headers_v(ve,',');
		long next_time = RTC_DS1307::now().unixtime()+READ_INTERVAL;

		printf("Initialized\n");
		int row_num = 0;
		while(temp_sensor_mode == TEMP_SENSOR_MODE_READ)
		{
			if(RTC_DS1307::now().unixtime()>=next_time)
			{
				row_num++;
				next_time += READ_INTERVAL;

				std::string str = probe->get_value();
				str = str.substr(0,str.find('\n'));

				dt = RTC_DS1307::now();
				ve[0] = dt.time_stamp();

				char buffer[17] = {};
				snprintf(buffer, 16, "%4d|", row_num);
				std::string row_line(buffer);

				lcd_screen->update_screen("Row#|Temp1,Temp2", row_line + str);

				double a,b;
				sscanf(str.c_str(),"%lf,%lf",&a,&b);
				
				{
					std::stringstream ss;
					ss << std::fixed << std::setprecision(2) << a;
					ve[1] = ss.str();
				}
				{
					std::stringstream ss;
					ss << std::fixed << std::setprecision(2) << b;
					ve[2] = ss.str();
				}
				ce.export_values_v(ve,',');
				
				printf("%s %.2lf %.2lf\n",ve[0].c_str(),a,b);
			}
		}
		printf("Done\n");
		return;
	}

	void start(void *args)
	{
		probe = new temp_sensor();
		lcd_screen = (lcd*) args;

		lcd_screen->reset_isr(DPIN_DOWN_DEFAULT, press_down);
		lcd_screen->reset_isr(DPIN_SELECT_DEFAULT, press_select);

		temp_sensor_mode = -1;
		while(true)
		{
			lcd_screen->update_screen(temp_func_names[temp_sensor_option], ((temp_sensor_option == option_size - 1)?(""):(temp_func_names[temp_sensor_option + 1])));
			while(temp_sensor_mode == -1)
			{
				sleep(1);
			}

			if(temp_sensor_mode == option_size - 1)
				break;
			else
			{
				temp_sensor_functions[temp_sensor_mode]();
				temp_sensor_mode = -1;
			}
			sleep(3);
		}

		lcd_screen->reset_isr(DPIN_DOWN_DEFAULT);
		lcd_screen->reset_isr(DPIN_SELECT_DEFAULT);
		delete probe;
		delete RTC;
	}
}

#endif