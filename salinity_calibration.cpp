#include "cond_probe.h"
#include "RTClib.h"

int main()
{
	RTC_DS1307::begin();
	cond_probe::begin();
	puts("HURRAH");
	cond_probe::calibrate(2);
	puts("HURRAH");
}