#include "mraa.hpp"
#include "temp_sensor.h"
#include "RTClib.h"
#include <cstdio>
#include <string>
#include <cmath>
#include "csv_export.h"
#include <sstream>
#include <vector>
#include <iomanip>
//#include <lcd_ui.h>

#define READ_INTERVAL 5		//minimum of 10
#define THRESHOLD 0.5

//intervals in array form
temp_sensor ts;

int main()
{
	RTC_DS1307::begin();
	//puts("CLOCK INITIALIZED");

	DateTime dt = RTC_DS1307::now();

	std::vector<std::string> ve;
	ve.push_back(std::string("Date/Time"));
	ve.push_back(std::string("Temp A"));
	ve.push_back(std::string("Temp B"));
	csv_export ce(std::string("temp_log.csv"),ve.size());

	ce.create_headers_v(ve,',');
	long next_time = RTC_DS1307::now().unixtime()+READ_INTERVAL;

	printf("Initialized\n");
	while(1)
	{
		if(RTC_DS1307::now().unixtime()>=next_time)
		{
			next_time += READ_INTERVAL;

			std::string str = ts.get_value();
			str = str.substr(0,str.find('\n'));

			dt = RTC_DS1307::now();
			ve[0] = dt.time_stamp();

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
				break;
		}
	}
	printf("Done\n");
}