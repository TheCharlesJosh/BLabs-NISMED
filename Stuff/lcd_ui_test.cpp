#include "lcd_ui.h"

void function1(void * args)
{
	lcd* lcd_screen = (lcd*) args;

	lcd_screen->update_screen("You picked", "Option 1");
	// RTC_DS1307::RTC_sleep(3);
	sleep(1);
	std::cout << "Option 1 finished\n";
}

void function2(void * args)
{
	lcd* lcd_screen = (lcd*) args;

	lcd_screen->update_screen("You picked", "Option 2");
	sleep(2);
	std::cout << "Option 2 finished\n";
}

void function3(void * args)
{
	lcd* lcd_screen = (lcd*) args;

	lcd_screen->update_screen("You picked", "Option 3");
	sleep(3);
	std::cout << "Option 3 finished\n";
}

int main()
{
	lcd* lcd_screen = new lcd();

	lcd_screen->add_option("OPTION 1", function1);
	lcd_screen->add_option("OPTION 2", function2);
	lcd_screen->add_option("OPTION 3", function3);
	lcd_screen->add_option("EXIT", NULL);

	lcd_screen->start();
	lcd_screen->update_screen("hello", "world");
	RTC_DS1307::RTC_sleep(3);
	delete lcd_screen;
}