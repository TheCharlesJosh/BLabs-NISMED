#include "jhd1313m1.h"
#include "mraa.hpp"
#include "temp_sensor.h"
#include "RTClib.h"
#include <time.h>
#include <unistd.h>
#include <iostream>
#include <string>
#include <cstdlib>
#include <vector>
#include <cstdio>
#include <utility>
#include <deque>
#include <cmath>

#define RGB_ADDRESS 0x62
#define LCD_ADDRESS 0x3E

//pin_no for up, down, select
#define DPIN_UP 8
#define DPIN_DOWN 7
#define DPIN_SELECT 6
#define DPIN_START 5

//pin_no for sensors
#define APIN_PH 0

void initialize();
void deinitialize();

int display_options();
void display_start();
void start_pH_sensor();
void start_Temp_sensor();
void calibrate();

void press_up(void *args);
void press_down(void *args);
void press_select(void *args);
void press_start(void *args);


upm::Jhd1313m1 *lcd;
mraa::Gpio *button_up;
mraa::Gpio *button_down;
mraa::Gpio *button_select;
mraa::Gpio *button_start;

volatile int display_row;
volatile int display_col;

int cursor_row;
int cursor_col;

/*
	current mode values - 
	0 -> display options
	1 -> pH sensor
	2 -> temperature 
	3 -> calibrate pH
*/

int current_mode;

#define DISPLAY_MODE 0
#define PH_SENSOR_MODE 1
#define TEMP_SENSOR_MODE 2
#define CALIBRATE_MODE 3

bool finished_display_mode;
bool finished_pH_Sensor_mode;
bool finished_Temp_Sensor_mode;

std::vector<std::string> options;

bool every_other_down = false;
bool every_other_up = false;
bool every_other_select = false;
bool every_other_start = false;

//<-- pH Sensor -->

#define CALIBRATE_TIME 60
#define NEUTRAL_PH 7.00

double pH_offset = 0;

//<-- pH Sensor -->

//<-- Temperature Sensor -->

#define READ_INTERVAL 5		//minimum of 10

typedef std::pair<double,double> dd;

//intervals in array form
int interv[] = {15,15};
temp_sensor ts;

//<-- Temperature Sensor -->

int main()
{
	int n;
	std::cin >> n;
	std::string temp;
	std::getline(std::cin, temp);

	for(int i = 0; i < n; i++)
	{
		std::getline(std::cin, temp);
		options.push_back(temp);
	}

	initialize();

	while(!finished_display_mode)
	{
		sleep(1);
		// std::cout << "\t" << button_start->read() << std::endl;
		switch(current_mode)
		{
			case DISPLAY_MODE:
				display_options();
				break;
			case PH_SENSOR_MODE:
				start_pH_sensor();
				break;
			case CALIBRATE_MODE:
				calibrate();
				break;
			case TEMP_SENSOR_MODE:
				start_Temp_sensor();
				break;
		}
	}

	deinitialize();
	return 0;
}

void initialize()
{
	button_up = new mraa::Gpio(DPIN_UP);
	button_down = new mraa::Gpio(DPIN_DOWN);
	button_select = new mraa::Gpio(DPIN_SELECT);
	button_start = new mraa::Gpio(DPIN_START);

	button_down->dir(mraa::DIR_IN);
	button_start->dir(mraa::DIR_IN);
	button_select->dir(mraa::DIR_IN);

	lcd = new upm::Jhd1313m1(0, LCD_ADDRESS, RGB_ADDRESS);
	
	lcd->displayOff();
	lcd->cursorOn();
	lcd->cursorBlinkOn();

	display_row = 0;
	display_col = 0;

	cursor_row = 0;
	cursor_col = 0;

	current_mode = DISPLAY_MODE;

	finished_display_mode = false;

	button_down->isr(mraa::EDGE_BOTH, &press_down, NULL);
	button_select->isr(mraa::EDGE_BOTH, &press_select, NULL);
	button_start->isr(mraa::EDGE_BOTH, &press_start, NULL);

	display_options();
}

void deinitialize()
{
	lcd->displayOff();

	button_down->isrExit();

	delete lcd;
	delete button_up;
	delete button_down;
	delete button_select;
}

int display_options()
{
	lcd->displayOn();
	lcd->clear();
	lcd->setCursor(cursor_row, cursor_col);
	lcd->write(options[display_row]);
	if(display_row < options.size() - 1)
	{
		lcd->setCursor(cursor_row+1, cursor_col);
		lcd->write(options[display_row+1]);
	}
	lcd->setCursor(cursor_row, cursor_col);

	return 1;
}

void press_down(void *args)
{
	//every other interrupt is ignored, quick hack because we can't use EDGE_RISING
	if(every_other_down ^= true)
		return;

	std::cout << "pressed down" << std::endl;

	if(current_mode == DISPLAY_MODE)
	{
		if(display_row < options.size() - 1)
			display_row++;
		else if(display_row == options.size() - 1)
			display_row = 0;

		display_options();
	}
}

void press_select(void *args)
{
	if(every_other_select ^= true)
		return;

	std::cout << "pressed select" << std::endl;

	if(current_mode == DISPLAY_MODE)
	{
		switch(display_row)
		{
			case 0:
				//pH Sensor
				current_mode = PH_SENSOR_MODE;
				break;
				break;
			case 1:
				//Temp Sensor
				current_mode = TEMP_SENSOR_MODE;
				break;
			case 2:
				//Calib Neut
				current_mode = CALIBRATE_MODE;
				break;
			case 3:
				finished_display_mode = true;
				deinitialize();
				exit(0);
				break;
		}
	}
	else
	{
		current_mode = DISPLAY_MODE;
		// switch(current_mode)
		// {
		// 	//exit to DISPLAY_MODE
		// 	case PH_SENSOR_MODE:
		// 		current_mode = DISPLAY_MODE;
		// 		break;
		// 	case TEMP_SENSOR_MODE:
		// 		current_mode = DISPLAY_MODE;
		// 		break;
		// }
	}
}

void press_start(void *args)
{
	if(every_other_start ^= true)
		return;

	std::cout << "pressed start" << std::endl;
	current_mode = DISPLAY_MODE;
	return;
}

void start_pH_sensor()
{
	mraa::Aio *ph_sensor = new mraa::Aio(APIN_PH);
	
	lcd->clear();
	lcd->setCursor(0,0);
    lcd->write("Row #|     Value");

    std::deque<double> pH_array;
    
    double running_total = 0;
    int row_num = 1;
    while(current_mode == PH_SENSOR_MODE)
    {
    	int value = ph_sensor->read();
        row_num++;

        pH_array.push_back(value*3.5*5.0/1024 + pH_offset);
        running_total += pH_array.back();
        if(row_num > CALIBRATE_TIME)
        {
        	running_total -= pH_array.front();
        	pH_array.pop_front();
        }

    	lcd->setCursor(1,0);
        char row_line[17] = {};
        sprintf(row_line, "%5d|%10lf", row_num, running_total/pH_array.size());
        sleep(1);

        lcd->write(std::string(row_line));
    }

    delete ph_sensor;
}

void start_Temp_sensor()
{
	RTC_DS1307::begin();
	lcd->clear();
	lcd->setCursor(0,0);
	lcd->write("Loading...");

	puts("CLOCK INITIALIZED");

	long next_time = RTC_DS1307::now().unixtime()+READ_INTERVAL;

	int row_num = 1;
	while(current_mode == TEMP_SENSOR_MODE)
	{
		if(RTC_DS1307::now().unixtime()>=next_time)
		{
			lcd->clear();
			lcd->setCursor(0,0);
			lcd->write("Row#|Temp1,Temp2");
			
			next_time += READ_INTERVAL;
			// printf("%d\n", current_mode);

			char row_line[17] = {};

			std::string str = ts.get_value();
			if(str == "")
				continue;

			snprintf(row_line, 16, "%4d|", row_num);
			lcd->setCursor(1,0);
			lcd->write(std::string(row_line));

			std::string value = str.substr(0,str.find('\n')+1);

			double temp_left, temp_right;

			// printf("%s\n", value.c_str());
			sscanf(value.c_str(), "%lf,%lf", &temp_left, &temp_right);
			printf("Read: %lf | %lf\nDifference: %lf\n", temp_left, temp_right, fabs(temp_right - temp_left));

			if(fabs(temp_right - temp_left) > 0.25)
			{
				lcd->write(value);
			}
			else
			{
				lcd->clear();
				lcd->setCursor(0,0);
				lcd->write("Left and Right");
				lcd->setCursor(1,0);
				lcd->write("Temp are Equal");
				sleep(3);
				current_mode = DISPLAY_MODE;
			}
			// lcd->write(std::string(row_line));

			row_num++;
		}

		RTC_DS1307::RTC_sleep(1);
	}
}

void calibrate()
{
	mraa::Aio *ph_sensor = new mraa::Aio(APIN_PH);
	
	lcd->clear();

	double total = 0;

    int row_num = 1;
    for(int i = 0; i < CALIBRATE_TIME && current_mode == CALIBRATE_MODE; i++)
    {
    	lcd->clear();
    	lcd->setCursor(0,0);

    	char row_line[17] = {};
    	sprintf(row_line, "%3d seconds left", CALIBRATE_TIME - i);
	    lcd->write(std::string(row_line));

	    int value = ph_sensor->read();
	    total += (double) (value*5.0/1024.00);

        row_num++;
        sleep(1);

        lcd->write(std::string(row_line));
    }

    total /= CALIBRATE_TIME;
    pH_offset = NEUTRAL_PH - total*3.5;

    lcd->clear();
    lcd->setCursor(0,0);
    lcd->write("Calibrated");
    lcd->setCursor(1,0);
    lcd->write("to pH 7.00");

    printf("Current Offset: %lf\n", pH_offset);
    sleep(1);

    current_mode = DISPLAY_MODE;
    delete ph_sensor;
}