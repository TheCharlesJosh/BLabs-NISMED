#ifndef SENSOR_GUI_H
#define SENSOR_GUI_H 

#include "jhd1313m1.h"
#include "mraa.hpp"
#include <unistd.h>
#include <iostream>
#include <string>
#include <vector>

/*
	add pin numbers for digital/analog I/O pins
*/

class sensor_gui
{
private:
	upm::Jhd1313m1 *lcd;
	mraa::Gpio *button_up;
	mraa::Gpio *button_down;
	mraa::Gpio *button_select;

	/*
		add class for sensors here
	*/

	int display_row;
	int display_col;

	int cursor_row;
	int cursor_col;

	int current_mode;

	std::vector<std::string> options;
public:
	sensor_gui(std::vector<std::string>);
	~sensor_gui();

	int display_options();

	void press_down();
};

void button_down_wrapper(mraa::Gpio *button_down)

#endif