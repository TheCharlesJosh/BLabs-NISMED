#include "RTClib.h"
#include <cmath>
#include <cstdint>
#include <csignal>
#include <cstring>

int main()
{
	if(mraa::init()==mraa::ERROR_PLATFORM_ALREADY_INITIALISED)
		puts("Already initialized");
	mraa::Gpio *cpin = new mraa::Gpio(10);
	cpin->dir(mraa::DIR_OUT_HIGH);

	mraa::Spi *spi = new mraa::Spi(0);
	spi->mode(mraa::SPI_MODE0);

	uint8_t tx[] = {0x0B,0x00,0x00};

	cpin->write(0);
	uint8_t *rx = spi->write(tx,(int)sizeof(tx));
	cpin->write(1);
	tx[0] = 0x0;

	if(rx==0)
	{
		puts("ERROR");
	}
	else
	{
		printf("%d %d\n\n",(int)sizeof(rx),(int)sizeof(tx));
		for(int i=0; i<sizeof(rx); i++)
			printf("%d\n",rx[i]);
		free(rx);
	}

	delete spi;
	delete cpin;
}