#ifndef COND_PROBE_H
#define COND_PROBE_H

#include "mraa.hpp"

//response codes
#define NO_DATA 255
#define PENDING 254
#define FAILED 2
#define SUCCESS 1

//commands
#define CALIBRATE 0
#define FACTORY_RESET 1
#define DEVICE_INFORMATION 2
#define K 3
#define LED 4
#define OUTPUT 5
#define READ 6
#define SERIAL 7
#define SLEEP 8
#define STATUS 9
#define TEMP_COMPESATION 10

#define COND_PROBE_ADDRESS 100

class cond_probe
{
private:
	static int get_response(mraa::I2c *i2c, uint8_t data[32]);
	static void send_command(mraa::I2c *i2c, const char command[]);
public:
	static void begin();//sets the parameters
	static void calibrate(int type);
	static double read();
};

#endif