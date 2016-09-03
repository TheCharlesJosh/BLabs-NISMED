#include <cstdio>
#include "mraa.hpp"
#include "analog_pin.h"
#include "RTClib.h"

#define ANEM_PIN 2
#define PLU_PIN 1

#define TIME_INTERV_SEC 1
#define TRESHOLD 250

analog_pin anem,plu;

int val_arr[] = {552,654,650,667,664,669,670,662,636,611};

long get_time()
{
	return RTC_DS1307::now().unixtime();
}

void wait_check(int _sec)
{
	//execute passive operations

	long end = get_time() + _sec;
	long curr;

	while((curr=get_time())<=end)
	{
		//do nothing for now
	}
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
		case 666:
		case 667:
		case 668:
		case 669:
		case 670:
			return 0;
	}

	return -1;
}

//Simply output the scanned raw inputs
void mode1()
{
	while(1)
	{
		printf("%d %d\n",anem.get_value(),plu.get_value());
	}
}

//Collect and report the cumulative sum of cycles per time period
void mode2()
{
	int prev = 0;
	while(1)
	{
		long next_time = get_time()+TIME_INTERV_SEC;

		int cnt = 0;
		while(get_time()<next_time)
		{
			int curr_state;
			if(anem.get_value()>TRESHOLD)
				curr_state = 1;
			else
				curr_state = 0;

			if(curr_state!=prev)
			{
				prev = curr_state;
				cnt++;
			}
		}

		printf("%d raw cycles.\n",cnt);
	}
}

//Set the time of the RTC
void mode3()
{
	DateTime dt;
	int y,m,d,hh,mm,ss;
	scanf("%d %d %d %d %d %d",&y,&m,&d,&hh,&mm,&ss);
	dt = DateTime(y,m,d,hh,mm,ss);
	RTC_DS1307::adjust(dt);
}

void mode4()
{
	while(1)
		printf("%ld\n",get_time());
}

void mode5()
{
	while(1)
	{
		printf("%d\n",plu.get_value());
		if(plu.get_value()==671)
			while(1);
	}
}

int main()
{
	RTC_DS1307::begin();
	/*
	rtc.begin();
	anem.set_pin(ANEM_PIN);
	plu.set_pin(PLU_PIN);

	mode1();
	*/

	mode3();
	mode4();
	return 0;
}