#include "lcd_ui.h"
#include "ph_sensor.h"
#include "temp_sensor.h"
#include <signal.h>
#include <stdlib.h>
#include <unistd.h>

namespace func1
{
	volatile int function1_option = 0;
	int function1_mode = 0;

	std::vector<std::string> option_names; // names to be displayed
	std::vector<void(*)(void *)> option_functions; // ptrs to functions being called when selected
												   // note: the function's argument should be a void ptr
												   // 		because that will contain a pointer to the lcd 


	void function1_down(void *args)
	{
		lcd* temp = (lcd*) args;

		if(temp->check_pressed_down())
			return;

		std::cout << "Pressed FUNCTION1_DOWN" << std::endl;
		func1::function1_option = 1;
	}

	void function1_select(void *args)
	{
		lcd* temp = (lcd*) args;

		if(temp->check_pressed_select())
			return;

		std::cout << "Pressed FUNCTION1_SELECT" << std::endl;
		func1::function1_option = 1;
	}

}

void function1(void * args)
{
	lcd* lcd_screen = (lcd*) args;

	lcd_screen->reset_isr(DPIN_DOWN_DEFAULT, func1::function1_down);
	lcd_screen->reset_isr(DPIN_SELECT_DEFAULT, func1::function1_select);

	int counter = 0;
	char temp_line[17] = {};
	while(func1::function1_option != 1)
	{
		sprintf(temp_line, "%d", counter);
		lcd_screen->update_screen("Seconds Elapsed", std::string(temp_line));

		sleep(1);
		counter++;
	}

	// lcd_screen->update_screen("You picked", "Option 1");
	// // RTC_DS1307::RTC_sleep(3);
	// sleep(1);
	// std::cout << "Option 1 finished\n";

	lcd_screen->reset_isr(DPIN_DOWN_DEFAULT);
	lcd_screen->reset_isr(DPIN_SELECT_DEFAULT);
	func1::function1_option = 0;
}

int main()
{
	RTC_DS1307::begin();
	std::cout << "CLOCK_INITIALIZED" << std::endl;
	
	lcd* lcd_screen = new lcd();

	lcd_screen->add_option("OPTION 1", function1);
	lcd_screen->add_option("TEMP_SENSOR", temp_sensor_func::start);
	lcd_screen->add_option("PH_SENSOR", ph_sensor_func::start);
	lcd_screen->add_option("EXIT", NULL);

	lcd_screen->start();
	lcd_screen->update_screen("hello", "world");
	sleep(1);
	
	delete lcd_screen;
}