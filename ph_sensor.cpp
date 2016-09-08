#ifndef PH_SENSOR
#define PH_SENSOR

#include "ph_sensor.h"
#include "RTClib.h"
#include <iomanip>
#include "csv_export.h"

ph_sensor::ph_sensor(int pin = 0)
{
	i_pin = new analog_pin(pin);
	neutral_phlevel = get_settings::get_variable("/home/root/lib/settings", "NEUTRAL_PH_LEVEL");
	acid_phlevel = get_settings::get_variable("/home/root/lib/settings", "ACID_PH_LEVEL");
	base_phlevel = get_settings::get_variable("/home/root/lib/settings", "BASE_PH_LEVEL");
}

ph_sensor::~ph_sensor()
{
	delete i_pin;
}

int ph_sensor::calibrate_neutral()
{
	// unsigned int cur_time = ph_sensor_func::RTC->unixtime();
	
	// while(cur_time == ph_sensor_func::RTC->unixtime());
	// cur_time = ph_sensor_func::RTC->unixtime();
	sleep(1);

	int total_PH = 0;
	for(int i = 0; i < CALIBRATION_TIME; i++)
	{
		if(ph_sensor_func::ph_sensor_mode == -1)
			return this->neutral_phlevel;

		total_PH += i_pin->get_value();

		// while(cur_time == ph_sensor_func::RTC->unixtime());
		// cur_time = ph_sensor_func::RTC->unixtime();
		sleep(1);
	}
	this->neutral_phlevel = total_PH/CALIBRATION_TIME;

	std::cout << "\tStoring Variable: " << get_settings::store_variable("/home/root/lib/settings", "NEUTRAL_PH_LEVEL", this->neutral_phlevel) << std::endl;
	return this->neutral_phlevel;
}

int ph_sensor::calibrate_acid()
{
	// unsigned int cur_time = ph_sensor_func::RTC->unixtime();
	
	// while(cur_time == ph_sensor_func::RTC->unixtime());
	// cur_time = ph_sensor_func::RTC->unixtime();
	sleep(1);

	int total_PH = 0;
	for(int i = 0; i < CALIBRATION_TIME; i++)
	{
		if(ph_sensor_func::ph_sensor_mode == -1)
			return this->acid_phlevel;
		
		total_PH += i_pin->get_value();

		// while(cur_time == ph_sensor_func::RTC->unixtime());
		// cur_time = ph_sensor_func::RTC->unixtime();
		sleep(1);
	}
	this->acid_phlevel = total_PH/CALIBRATION_TIME;

	std::cout << "\tStoring Variable: " << get_settings::store_variable("/home/root/lib/settings", "ACID_PH_LEVEL", this->acid_phlevel) << std::endl;
	return this->acid_phlevel;
}

int ph_sensor::calibrate_base()
{
	// unsigned int cur_time = ph_sensor_func::RTC->unixtime();
	
	// while(cur_time == ph_sensor_func::RTC->unixtime());
	// cur_time = ph_sensor_func::RTC->unixtime();
	sleep(1);

	int total_PH = 0;
	for(int i = 0; i < CALIBRATION_TIME; i++)
	{
		if(ph_sensor_func::ph_sensor_mode == -1)
			return this->base_phlevel;
		
		total_PH += i_pin->get_value();

		// while(cur_time == ph_sensor_func::RTC->unixtime());
		// cur_time = ph_sensor_func::RTC->unixtime();
		sleep(1);
	}
	this->base_phlevel = total_PH/CALIBRATION_TIME;

	std::cout << "\tStoring Variable: " << get_settings::store_variable("/home/root/lib/settings", "BASE_PH_LEVEL", this->base_phlevel) << std::endl;
	return this->base_phlevel;
}

double ph_sensor::get_PHvalue()
{
	int temp = i_pin->get_value();

	double slope = (PH_NEUTRAL - PH_ACIDIC)/(this->neutral_phlevel - this->acid_phlevel);
	std::cout << "\tPH_SENSOR: \n\t\toffset == " << temp - this->neutral_phlevel << std::endl << "\t\tslope == " << slope << std::endl;
	return PH_NEUTRAL + slope*(temp - this->neutral_phlevel);

	//disabled calibrate base
	// if(temp < this->neutral_phlevel)
	// {
	// 	//calculate ph for acid
	// 	double slope = (PH_NEUTRAL - PH_ACIDIC)/(this->neutral_phlevel - this->acid_phlevel);
	// 	std::cout << "\tPH_SENSOR: \n\t\toffset == " << temp - this->neutral_phlevel << std::endl << "\t\tslope == " << slope << std::endl;
	// 	return PH_NEUTRAL + slope*(temp - this->neutral_phlevel);
	// }
	// else
	// {
	// 	//calculate ph for basic
	// 	double slope = (PH_BASIC - PH_NEUTRAL)/(this->base_phlevel - this->neutral_phlevel);
	// 	std::cout << "\tPH_SENSOR: \n\t\toffset == " << temp - this->neutral_phlevel << std::endl << "\t\tslope == " << slope << std::endl;
	// 	return slope*(temp - this->neutral_phlevel) + PH_NEUTRAL;
	// }
}

namespace ph_sensor_func
{
	ph_sensor* probe = NULL;
	lcd* lcd_screen = NULL;
	DateTime* RTC = NULL;

	const int option_size = 5;
	const std::string ph_func_names[] = {"Calib. Base", "Calib. Acid", "Calib. Neut", "Live Read", "Return"};
	volatile int ph_sensor_option = 0;
	int ph_sensor_mode = PH_SENSOR_SEL_MODE;
	int row_line = 0;
	void(*ph_sensor_functions[option_size])(void) = {calibrate_base, calibrate_acid, calibrate_neutral, read, NULL};

	void press_down(void *args)
	{
		if(lcd_screen->check_pressed_down())
			return;

		if(ph_sensor_mode != PH_SENSOR_SEL_MODE)
			return;

		ph_sensor_option++;
		if(ph_sensor_option >= option_size)
			ph_sensor_option = 0;

		lcd_screen->update_screen(ph_func_names[ph_sensor_option], ((ph_sensor_option == option_size - 1)?(""):(ph_func_names[ph_sensor_option + 1])));
		std::cout << "Pressed ph_sensor_down" << std::endl;
	}

	void press_select(void *args)
	{
		if(lcd_screen->check_pressed_select())
			return;

		// if in sub-option, returns to new option, else goes in sub-option
		if(ph_sensor_mode != PH_SENSOR_SEL_MODE)
			ph_sensor_mode = PH_SENSOR_SEL_MODE;
		else
			ph_sensor_mode = ph_sensor_option;

		std::cout << "Pressed ph_sensor_select" << std::endl;
	}

	void calibrate_base()
	{
		int temp = probe->calibrate_base();
		char buffer[100] = {};
		sprintf(buffer, "Level: %d", temp);
		lcd_screen->update_screen("Calib. Base", std::string(buffer));
		sleep(2);

		std::cout << "Calibrated with a value of " << temp << std::endl;
	}
	void calibrate_acid()
	{
		int temp = probe->calibrate_acid();
		char buffer[100] = {};
		sprintf(buffer, "Level: %d", temp);
		lcd_screen->update_screen("Calib. Base", std::string(buffer));
		sleep(2);

		std::cout << "Calibrated with a value of " << temp << std::endl;
	}
	void calibrate_neutral()
	{
		int temp = probe->calibrate_neutral();
		char buffer[100] = {};
		sprintf(buffer, "Level: %d", temp);
		lcd_screen->update_screen("Calib. Base", std::string(buffer));
		sleep(2);
		
		std::cout << "Calibrated with a value of " << temp << std::endl;
	}

	void read()
	{

		std::cout << "READING PH LEVEL" << std::endl;


		DateTime dt = RTC_DS1307::now();

		std::vector<std::string> row;
		row.push_back(std::string("Date/Time"));
		row.push_back(std::string("pH Level"));
		csv_export ce(std::string("ph_read.csv"),row.size());
		
		while(ph_sensor_mode == PH_SENSOR_READ)
		{
			row_line++;

			dt = RTC_DS1307::now();
			row[0] = dt.time_stamp();

			double read_PH = probe->get_PHvalue();
			char out_string[17] = {};
			sprintf(out_string, "%5d|%10lf", row_line, read_PH);
			std::cout << read_PH << std::endl;
			lcd_screen->update_screen("Row #|     Value", std::string(out_string));

			{
				std::stringstream ss;
				ss << std::fixed << std::setprecision(2) << read_PH;
				row[1] = ss.str();
			}
			ce.export_values_v(row, ',');

			sleep(1);
		}
		row_line = 0;
	}

	void start(void *args)
	{
		probe = new ph_sensor();
		lcd_screen = (lcd*) args;
		RTC = new DateTime(0);

		lcd_screen->reset_isr(DPIN_DOWN_DEFAULT, press_down);
		lcd_screen->reset_isr(DPIN_SELECT_DEFAULT, press_select);
		
		ph_sensor_option = 0;
		// unsigned int cur_time = RTC->unixtime();
		while(true)
		{
			lcd_screen->update_screen(ph_func_names[ph_sensor_option], ((ph_sensor_option == option_size - 1)?(""):(ph_func_names[ph_sensor_option + 1])));
			while(ph_sensor_mode == PH_SENSOR_SEL_MODE)
			{
				std::cout << "\t" << ph_sensor_option << " " << ph_sensor_mode << "\t" << RTC->unixtime() << std::endl;

				// while(cur_time == RTC->unixtime());
				// cur_time = RTC->unixtime();
				sleep(1);
			}

			if(ph_sensor_mode == option_size - 1)
				break;

			if(ph_sensor_mode == 3)
			{
				ph_sensor_functions[ph_sensor_mode]();
				
				// while(cur_time == RTC->unixtime());
				// cur_time = RTC->unixtime();
				sleep(1);
			}
			else
			{
				row_line = 0;

				lcd_screen->update_screen("Calibrating...", "Please wait");
				ph_sensor_functions[ph_sensor_mode]();

				lcd_screen->update_screen("Finished", "Calibration");
				std::cout << "Finished Calibration" << std::endl;
				ph_sensor_mode = PH_SENSOR_SEL_MODE;
				
				// cur_time = RTC->unixtime();
				// while(cur_time == RTC->unixtime());
				// cur_time = RTC->unixtime();
				sleep(1);
			}
		}

		ph_sensor_mode = -1;

		lcd_screen->reset_isr(DPIN_DOWN_DEFAULT);
		lcd_screen->reset_isr(DPIN_SELECT_DEFAULT);
		delete probe;
		delete RTC;
	}
}

#endif