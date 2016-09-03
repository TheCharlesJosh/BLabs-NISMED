#include "mraa.hpp"
#include "temp_sensor.h"
#include "RTClib.h"
#include <cstdio>
#include <utility>
#include <deque>

//define commands for command_queue
#define READ1 0			//has a delay of around 7 seconds total
#define OUTPUT 2

//define intervals for each command
#define READ_INTERVAL 15		//minimum of 10
#define OUTPUT_INTERVAL 15

#define READ_QUEUE_LENGTH 5

typedef std::pair<int,long> ii;

//intervals in array form
int interv[] = {15,15,15};
RTC_DS1307 rtc;

int main()
{
	rtc.begin();
	temp_sensor ts;

	std::deque<ii> command_queue;
	std::deque<double> read_queue[2];
	double total[2];
	for(int i=0; i<2; i++)
		total[i] = 0;

	long curr_time = rtc.now().unixtime();

	int pos = 0;
	while(pos<READ_QUEUE_LENGTH)
	{
		if(rtc.now().unixtime()>=curr_time)
		{
			pos++;
			curr_time += READ_INTERVAL;

			for(int i=0; i<2; i++)
			{
				if(i)
					rtc.RTC_sleep(1);
				double ret = ts.get_value(i+'0');
				read_queue[i].push_back(ret);
				total[i] += ret;
			}

		}
	}

	curr_time = rtc.now().unixtime();

	//output should always be first in the queue
	command_queue.push_back(ii(OUTPUT,curr_time));
	command_queue.push_back(ii(READ1,curr_time));

	while(1)
	{
		if(rtc.now().unixtime()>=command_queue.front().second)
		{
			int c = command_queue.front().first;
			long t = command_queue.front().second;
			command_queue.pop_front();
			command_queue.push_back(ii(c,t+interv[c]));

			if(c==OUTPUT)
			{
				printf("%.2lf %.2lf\n",total[0]/READ_QUEUE_LENGTH,total[1]/READ_QUEUE_LENGTH);
			}
			else if(c==READ1)
			{
				for(int i=0; i<2; i++)
				{
					if(i)
						rtc.RTC_sleep(1);
					double ret = ts.get_value('0'+i);
					total[i] -= read_queue[i].front();
					total[i] += ret;
					read_queue[i].pop_front();
					read_queue[i].push_back(ret);
				}
			}
		}
	}
}