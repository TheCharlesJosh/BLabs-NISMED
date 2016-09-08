/*
	I USE THE PRIMITIVE LCD INTERFACE
*/

#include "jhd1313m1.h"
#include "analog_pin.h"
#include "RTClib.h"
#include <cstdio>
#include <string>
#include <algorithm>
#include <cmath>
#include <iostream>

#define WV_PIN 1
#define ANEM_PIN 2
#define GAUGE_PIN 0

#define COLLECT_INTERVAL 1 //3 //default value is 3
#define K2 0.28	//taken from the dimension guide
#define K 0.205
#define NUM_SECTIONS 7
#define THRESHOLD 350

analog_pin wv(WV_PIN);
analog_pin anem(ANEM_PIN);
analog_pin pluv(GAUGE_PIN);
int arr[NUM_SECTIONS];
double center[] = {348.75,45.00,90.00,135.00,168.75,225.00,292.5};
double weight[] = {0.33,0.5,0.5,0.5,1,0.25,0.33};
double adj[] = {1,1,1,1};
double fact[4][NUM_SECTIONS];
double error[4];
char equiv[] = "NWSE";


//used for the LCD
upm::Jhd1313m1 *screen;

//constants for the Grove LCD
#define RGB_ADDRESS 0x62
#define LCD_ADDRESS 0x3E

void init()
{
	RTC_DS1307::begin();
	for(int i=0; i<4; i++)
		for(int j=0; j<NUM_SECTIONS; j++)
			fact[i][j] = weight[j]*pow(std::min(abs(i*90.00-center[j]),abs(i*90.00-(center[j]-360.00))),2);
}

int get_section(int n)
{
	switch(n)
	{
		case 550:
		case 551:
		case 552:
			return 4;
		case 608:
		case 609:
		case 610:
			return 3;
		case 634:
		case 635:
		case 636:
			return 2;
		case 652:
		case 653:
		case 654:
			return 5;
		case 661:
		case 662:
		case 663:
			return 1;
		case 665:
		case 666:
		case 667:
			return 6;
		case 668:
		case 669:
		case 670:
			return 0;
	}

	return -1;
}

void update_screen(std::string upper, std::string lower)
{
	screen->clear();
	screen->setCursor(0, 0);
	screen->write(upper);
	screen->setCursor(1, 0);
	screen->write(lower);
	screen->setCursor(0, 0);
}

int main()
{
	//initialize lcd and sensors

	screen = new upm::Jhd1313m1(0, LCD_ADDRESS, RGB_ADDRESS);
	screen->displayOn();
	screen->cursorOn();
	screen->cursorBlinkOn();

	update_screen("Please Wait...", "Initializing");

	init();

	int prev = anem.get_value()>THRESHOLD?1:0;
	int prev2 = pluv.get_value()>THRESHOLD?1:0;
	while(1)
	{
		for(int i=0; i<NUM_SECTIONS; i++)
			arr[i] = 0;
		long next_time = RTC_DS1307::now().unixtime()+COLLECT_INTERVAL;
		int cnt = 0;
		int cnt2 = 0;
		while(RTC_DS1307::now().unixtime()<next_time)
		{
			int curr = anem.get_value()>THRESHOLD?1:0;
			if(curr!=prev)
			{
				cnt++;
				prev = curr;
			}

			int sec = get_section(wv.get_value());
			if(sec>=0)
				arr[sec]++;

			curr = pluv.get_value()>THRESHOLD?1:0;
			if(curr!=prev2)
			{
				cnt2++;
				prev2 = curr;
			}
		}

		for(int i=0; i<4; i++)
		{
			error[i] = 0;
			for(int j=0; j<NUM_SECTIONS; j++)
				error[i] += arr[j]*fact[i][j];
		}

		int dir = 0;
		double minerr = error[0];
		for(int i=1; i<4; i++)
			if(error[i]<minerr)
			{
				minerr = error[i];
				dir = i;
			}
		/*
		for(int i=0; i<NUM_SECTIONS; i++)
			printf("%d  ",arr[i]);
		puts("");
		for(int i=0; i<4; i++)
			printf("%.3lf  ",error[i]);
		puts("");
		*/
		printf("WIND VELOCITY: %.3lf m/s %c\nRAINFALL RATE: %.3lf mm/s\n\n",(double)cnt/COLLECT_INTERVAL*K, equiv[dir],(double)cnt2/COLLECT_INTERVAL*K2);

		char buffer[17] = {};
		sprintf(buffer, "WIND %6.3lfm/s %c", (double)cnt/COLLECT_INTERVAL*K, equiv[dir]);
		std::string upper(buffer);
		sprintf(buffer, "RAIN %6.3lfmm/s", (double)cnt2/COLLECT_INTERVAL*K2);
		std::string lower(buffer);

		std::cout << "\t" << upper << "\t" << lower << std::endl;

		update_screen(upper, lower);
	}

	//turn off lcd
	screen->displayOff();
	delete screen;

	return 0;
}