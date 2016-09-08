#include "sensor_gui.h"

#define RGB_ADDRESS 0x62
#define LCD_ADDRESS 0x3E

//pin_no for up, down, select
#define DIGITAL_PIN_UP 2
#define DPIN_DOWN 3
#define DPIN_SELECT 4

sensor_gui::sensor_gui(std::vector<std::string> input_options)
{
	button_up = new mraa::Gpio(DIGITAL_PIN_UP);
	button_down = new mraa::Gpio(DPIN_DOWN);
	button_select = new mraa::Gpio(DPIN_SELECT);

	button_down->dir(mraa::DIR_IN);

	lcd = new upm::Jhd1313m1(0, LCD_ADDRESS, RGB_ADDRESS);
	
	lcd->displayOff();
	lcd->cursorOn();
	lcd->cursorBlinkOn();

	button_down->isr(mraa::EDGE_RISING, &button_down_wrapper, &button_down);

	display_row = 0;
	display_col = 0;

	cursor_row = 0;
	cursor_col = 0;

	current_mode = 0;

	options = input_options;
	display_options();
}

sensor_gui::~sensor_gui()
{
	lcd->displayOff();

	button_down->isrExit();

	delete lcd;
	delete button_up;
	delete button_down;
	delete button_select;
}

int sensor_gui::display_options()
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

void sensor_gui::press_down()
{
	if(display_row < options.size() - 1)
		display_row++;
	else if(display_row == options.size() - 1)
		display_row = 0;

	display_options();
}