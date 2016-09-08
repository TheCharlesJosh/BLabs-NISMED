#ifndef LCD_UI_H
#define LCD_UI_H

#include "jhd1313m1.h"
#include "mraa.hpp"
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
#include <sstream>

//constants for the Grove LCD
#define RGB_ADDRESS 0x62
#define LCD_ADDRESS 0x3E

//pin_no for up, down, select
#define DPIN_DOWN_DEFAULT 7
#define DPIN_SELECT_DEFAULT 6

#define EXIT_MODE 3
#define START_MODE -1

class lcd
{
private:
	std::vector<std::string> option_names; // names to be displayed
	std::vector<void(*)(void *)> option_functions; // ptrs to functions being called when selected
												   // note: the function's argument should be a void ptr
												   // 		because that will contain a pointer to the lcd 

	upm::Jhd1313m1 *screen;
	mraa::Gpio *button_down;
	mraa::Gpio *button_select;
/*
	current mode values - 
	0 -> display options
*/

	int current_mode;

	volatile int current_option;

	static void press_down(void*);
	static void press_select(void*);

	bool isr_set; //1 if start() has been called
public:
	lcd(int DPIN_DOWN = DPIN_DOWN_DEFAULT, int DPIN_SELECT = DPIN_SELECT_DEFAULT);
	~lcd();
	
	void add_option(std::string, void(*)(void *)); // adds option on main menu, and the function to be launched when the button is pressed

	void start(); // turns the lcd on in a loop

	void update_screen(std::string, std::string); // updates screen

	void reset_isr(int); // set isr to default option
	void reset_isr(int, void(*)(void *));

	bool check_pressed_down(); //check if state of down button is at the rising edge
	bool check_pressed_select(); //check if state of select button is at the rising edge
};

#endif