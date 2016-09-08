#ifndef LCD_UI
#define LCD_UI

#include "lcd_ui.h"

lcd::lcd(int DPIN_DOWN, int DPIN_SELECT)
{
	option_names.clear();
	option_functions.clear();

	screen = new upm::Jhd1313m1(0, LCD_ADDRESS, RGB_ADDRESS);
	screen->displayOff();
	screen->cursorOn();
	screen->cursorBlinkOn();

	button_down = new mraa::Gpio(DPIN_DOWN);
	button_select = new mraa::Gpio(DPIN_SELECT);

	button_down->dir(mraa::DIR_IN);
	button_select->dir(mraa::DIR_IN);

	current_mode = START_MODE;
	current_option = 0;

	isr_set = false;
}
lcd::~lcd()
{
	screen->displayOff();
	delete screen;
	delete button_down;
	delete button_select;
}

void lcd::update_screen(std::string upper, std::string lower)
{
	screen->clear();
	screen->setCursor(0, 0);
	screen->write(upper);
	screen->setCursor(1, 0);
	screen->write(lower);
	screen->setCursor(0, 0);
}

void lcd::add_option(std::string option_name, void(*func_ptr)(void *))
{
	option_names.push_back(option_name);
	option_functions.push_back(func_ptr);
}

void lcd::start()
{
	this->reset_isr(DPIN_DOWN_DEFAULT);
	this->reset_isr(DPIN_SELECT_DEFAULT);

	isr_set = true;

	this->screen->displayOn();

	if(this->option_names.size() == 0)
		this->update_screen("No Options", "Pin6 to exit");
	else
		this->update_screen(this->option_names[0], (this->option_names.size() > 1)?this->option_names[1]:"");
	
	while(true)
	{
		if(this->current_mode == EXIT_MODE)
			break;

		if(this->current_mode != START_MODE)
		{
			this->option_functions[(this->current_mode)](this);
			std::cout << "Finished option #" << this->current_mode << std::endl;
			this->current_mode = START_MODE;

			this->reset_isr(DPIN_DOWN_DEFAULT);
			this->reset_isr(DPIN_SELECT_DEFAULT);

			this->update_screen(this->option_names[this->current_option], (this->option_names.size() > 1)?this->option_names[this->current_option + 1]:"");
		}
		// RTC_DS1307::RTC_sleep(1);
		sleep(1);

		std::cout << this->current_mode << " " << this->current_option << std::endl;
	}
}

void lcd::press_down(void* args)
{
	lcd* temp = (lcd*) args;

	if(temp->button_down->read() == 0)
		return;

	std::cout << "Pressed DEFAULT_DOWN" << std::endl;

	(temp->current_option)++;

	if(temp->current_option > ((int) temp->option_names.size()) - 1)
		temp->current_option = 0;

	temp->update_screen(temp->option_names[temp->current_option], (temp->current_option == ((int) temp->option_names.size()) - 1)?"":temp->option_names[temp->current_option + 1]);
}

void lcd::press_select(void* args)
{
	lcd* temp = (lcd*) args;

	if(temp->button_select->read() == 0)
		return;

	std::cout << "Pressed DEFAULT_SELECT" << std::endl;

	if(temp->option_functions[temp->current_mode] == NULL)
	{
		temp->current_mode = EXIT_MODE;
		return;
	}
	else
		temp->current_mode = temp->current_option;
}

void lcd::reset_isr(int pin)
{
	mraa::Gpio *button;
	
	switch(pin)
	{
		case DPIN_DOWN_DEFAULT:
			button = this->button_down;
			break;
		case DPIN_SELECT_DEFAULT:
			button = this->button_select;
			break;
	}

	if(isr_set)
	{
		std::cout << "ISR EXIT FOR PIN " << pin << std::endl;
		button->isrExit();
	}

	switch(pin)
	{
		case DPIN_DOWN_DEFAULT:
			std::cout << "DEFAULT ISR SET FOR PIN" << pin << std::endl;
			button->isr(mraa::EDGE_BOTH, &(lcd::press_down), this);
			break;
		case DPIN_SELECT_DEFAULT:
			std::cout << "DEFAULT ISR SET FOR PIN" << pin << std::endl;
			button->isr(mraa::EDGE_BOTH, &(lcd::press_select), this);
			break;
	}
}

void lcd::reset_isr(int pin, void(*fptr)(void *))
{
	mraa::Gpio *button;
	
	switch(pin)
	{
		case DPIN_DOWN_DEFAULT:
			button = this->button_down;
			break;
		case DPIN_SELECT_DEFAULT:
			button = this->button_select;
			break;
	}

	if(isr_set)
	{
		std::cout << "ISR EXIT FOR PIN " << pin << std::endl;
		button->isrExit();
	}

	switch(pin)
	{
		case DPIN_DOWN_DEFAULT:
			button->isr(mraa::EDGE_BOTH, fptr, this);
			std::cout << "CUSTOM ISR SET FOR PIN" << pin << std::endl;
			break;
		case DPIN_SELECT_DEFAULT:
			button->isr(mraa::EDGE_BOTH, fptr, this);
			std::cout << "CUSTOM ISR SET FOR PIN" << pin << std::endl;
			break;
	}
}

bool lcd::check_pressed_down()
{
	return this->button_down->read() == 0;
}

bool lcd::check_pressed_select()
{
	return this->button_select->read() == 0;
}

#endif