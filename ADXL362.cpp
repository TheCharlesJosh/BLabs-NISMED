/*
 Arduino Library for Analog Devices ADXL362 - Micropower 3-axis accelerometer
 go to http://www.analog.com/ADXL362 for datasheet
 
 
 License: CC BY-SA 3.0: Creative Commons Share-alike 3.0. Feel free 
 to use and abuse this code however you'd like. If you find it useful
 please attribute, and SHARE-ALIKE!
 
 Created June 2012
 by Anne Mahaffey - hosted on http://annem.github.com/ADXL362

 Modified May 2013
 by Jonathan Ruiz de Garibay
 
 */ 

#include "ADXL362.h"
#include "mraa.hpp"
#include <cstdint>
#include <csignal>
#include "RTClib.h"

//#define ADXL362_DEBUG

int16_t slaveSelectPin = 10;

ADXL362::ADXL362(){
}


//
//  begin()
//  Initial SPI setup, soft reset of device
//
void ADXL362::begin(int16_t chipSelectPin) {
	slaveSelectPin = chipSelectPin;
	cspin = new mraa::Gpio(slaveSelectPin);
	cspin->dir(mraa::DIR_OUT);
	if(cspin->write(1)!=mraa::SUCCESS)
		printf("ERROR\n");

	spi = new mraa::Spi(0);
	if(spi->mode(mraa::SPI_MODE0)!=mraa::SUCCESS)
		printf("ERROR\n");
	usleep(1000000);
    
	// soft reset
	SPIwriteOneRegister(0x1F, 0x52);  // Write to SOFT RESET, "R"
	usleep(10000);
#ifdef ADXL362_DEBUG
	printf("Soft Reset\n");
#endif
 }
 
//
//  beginMeasure()
//  turn on Measurement mode - required after reset
// 
void ADXL362::beginMeasure() {
	uint8_t temp = SPIreadOneRegister(0x2D);	// read Reg 2D before modifying for measure mode

#ifdef ADXL362_DEBUG
	printf(  "Setting Measeurement Mode - Reg 2D before = "); 
	printf("%d",temp); 
#endif

	// turn on measurement mode
	uint8_t tempwrite = temp|0x02;			// turn on measurement bit in Reg 2D
	SPIwriteOneRegister(0x2D, tempwrite); // Write to POWER_CTL_REG, Measurement Mode
	RTC_DS1307::RTC_sleep(1);
  
#ifdef ADXL362_DEBUG
	temp = SPIreadOneRegister(0x2D);
	printf(  ", Reg 2D after = "); 
	printf("%d\n\n",temp); 
#endif
}

//
//  readXData(), readYData(), readZData(), readTemp()
//  Read X, Y, Z, and Temp registers
//
int16_t ADXL362::readXData(){
	int16_t XDATA = SPIreadTwoRegisters(0x0E);
	
#ifdef ADXL362_DEBUG
	printf("XDATA = ");
	printf("%d\n",XDATA);
#endif
	
	return XDATA;
}

int16_t ADXL362::readYData(){
	int16_t YDATA = SPIreadTwoRegisters(0x10);

#ifdef ADXL362_DEBUG
	printf("\tYDATA = "); 
	printf("%d\n",YDATA);
#endif
	
	return YDATA;
}

int16_t ADXL362::readZData(){
	int16_t ZDATA = SPIreadTwoRegisters(0x12);

#ifdef ADXL362_DEBUG
	printf("\tZDATA = "); 
	printf("%d\n",ZDATA);
#endif

	return ZDATA;
}

int16_t ADXL362::readTemp(){
	int16_t TEMP = SPIreadTwoRegisters(0x14);

#ifdef ADXL362_DEBUG
	printf("\tTEMP = "); 
	printf("%d\n",TEMP);
#endif

	return TEMP;
}

void ADXL362::readXYZData(int16_t &XData, int16_t &YData, int16_t &ZData){
	  // burst SPI read
	  // A burst read of all three axis is required to guarantee all measurements correspond to same sample time

	uint8_t tx[] = {0x0B,0x0E,0x00,0x00,0x00,0x00,0x00,0x00};
	uint8_t rx[sizeof(tx)];
	for(int i=0; i<sizeof(rx); i++)
		rx[i] = 0x00;

	cspin->write(0);
	if(spi->transfer(tx,rx,sizeof(tx))!=mraa::SUCCESS)
		printf("ERROR IN readXYZData\n");
	cspin->write(1);
	XData = (rx[3]<<8)|rx[2];
	YData = (rx[5]<<8)|rx[4];
	ZData = (rx[7]<<8)|rx[6];

  
#ifdef ADXL362_DEBUG
	printf("XDATA = "); printf("%d\n",XData); 
	printf("\tYDATA = "); printf("%d\n",YData); 
	printf("\tZDATA = "); printf("%d\n",ZData); 
#endif
}

void ADXL362::setupDCActivityInterrupt(int16_t threshold, uint8_t _time){
	//  Setup motion and time thresholds
	SPIwriteTwoRegisters(0x20, threshold);
	SPIwriteOneRegister(0x22, _time);

	// turn on activity interrupt
	uint8_t ACT_INACT_CTL_Reg = SPIreadOneRegister(0x27);  // Read current reg value
	ACT_INACT_CTL_Reg = ACT_INACT_CTL_Reg | (0x01);     // turn on bit 1, ACT_EN  
	SPIwriteOneRegister(0x27, ACT_INACT_CTL_Reg);       // Write new reg value 
	ACT_INACT_CTL_Reg = SPIreadOneRegister(0x27);       // Verify properly written

#ifdef ADXL362_DEBUG
	printf("DC Activity Threshold set to ");  	printf("%d",SPIreadTwoRegisters(0x20));
	printf(", Time threshold set to ");  		printf("%d",SPIreadOneRegister(0x22)); 
	printf(", ACT_INACT_CTL Register is ");  	printf("%x\n",ACT_INACT_CTL_Reg);
#endif
}

void ADXL362::setupACActivityInterrupt(int16_t threshold, uint8_t _time){
	//  Setup motion and time thresholds
	SPIwriteTwoRegisters(0x20, threshold);
	SPIwriteOneRegister(0x22, _time);
  
	// turn on activity interrupt
	uint8_t ACT_INACT_CTL_Reg = SPIreadOneRegister(0x27);  // Read current reg value
	ACT_INACT_CTL_Reg = ACT_INACT_CTL_Reg | (0x03);     // turn on bit 2 and 1, ACT_AC_DCB, ACT_EN  
	SPIwriteOneRegister(0x27, ACT_INACT_CTL_Reg);       // Write new reg value 
	ACT_INACT_CTL_Reg = SPIreadOneRegister(0x27);       // Verify properly written

#ifdef ADXL362_DEBUG
	printf("AC Activity Threshold set to ");  	printf("%d",SPIreadTwoRegisters(0x20));
	printf(", Time Activity set to ");  		printf("%d",SPIreadOneRegister(0x22));  
	printf(", ACT_INACT_CTL Register is ");  	printf("%x\n",ACT_INACT_CTL_Reg);
#endif
}

void ADXL362::setupDCInactivityInterrupt(int16_t threshold, int16_t _time){
	// Setup motion and time thresholds
	SPIwriteTwoRegisters(0x23, threshold);
	SPIwriteTwoRegisters(0x25, _time);

	// turn on inactivity interrupt
	uint8_t ACT_INACT_CTL_Reg = SPIreadOneRegister(0x27);   // Read current reg value 
	ACT_INACT_CTL_Reg = ACT_INACT_CTL_Reg | (0x04);      // turn on bit 3, INACT_EN  
	SPIwriteOneRegister(0x27, ACT_INACT_CTL_Reg);        // Write new reg value 
	ACT_INACT_CTL_Reg = SPIreadOneRegister(0x27);        // Verify properly written

#ifdef ADXL362_DEBUG
	printf("DC Inactivity Threshold set to ");  printf("%d",SPIreadTwoRegisters(0x23));
	printf(", Time Inactivity set to ");  printf("%d",SPIreadTwoRegisters(0x25));
	printf(", ACT_INACT_CTL Register is ");  printf("%x\n",ACT_INACT_CTL_Reg);
#endif
}

void ADXL362::setupACInactivityInterrupt(int16_t threshold, int16_t _time){
	//  Setup motion and time thresholds
	SPIwriteTwoRegisters(0x23, threshold);
	SPIwriteTwoRegisters(0x25, _time);
 
	// turn on inactivity interrupt
	uint8_t ACT_INACT_CTL_Reg = SPIreadOneRegister(0x27);   // Read current reg value
	ACT_INACT_CTL_Reg = ACT_INACT_CTL_Reg | (0x0C);      // turn on bit 3 and 4, INACT_AC_DCB, INACT_EN  
	SPIwriteOneRegister(0x27, ACT_INACT_CTL_Reg);        // Write new reg value 
	ACT_INACT_CTL_Reg = SPIreadOneRegister(0x27);        // Verify properly written

#ifdef ADXL362_DEBUG
	printf("AC Inactivity Threshold set to ");  printf("%d",SPIreadTwoRegisters(0x23));
	printf(", Time Inactivity set to ");  printf("%d",SPIreadTwoRegisters(0x25)); 
	printf(", ACT_INACT_CTL Register is ");  printf("%x\n",ACT_INACT_CTL_Reg);
#endif
}

void ADXL362::checkAllControlRegs(){
	//uint8_t filterCntlReg = SPIreadOneRegister(0x2C);
	//uint8_t ODR = filterCntlReg & 0x07;  Serial.print("ODR = ");  Serial.println(ODR, HEX);
	//uint8_t ACT_INACT_CTL_Reg = SPIreadOneRegister(0x27);      Serial.print("ACT_INACT_CTL_Reg = "); Serial.println(ACT_INACT_CTL_Reg, HEX);
#ifdef ADXL362_DEBUG
	printf("Start Burst Read of all Control Regs - Library version 6-5-2014:\n");
	for(int i=0; i<15; i++)
		printf("Reg %x = %x\n",0x20+i,SPIreadOneRegister(0x20+i));
#endif
	cspin->write(1);
}

// Basic SPI routines to simplify code
// read and write one register
uint8_t ADXL362::SPIreadOneRegister(uint8_t regAddress){
	uint8_t regValue = 0;

	uint8_t tx[] = {0x0B,regAddress,0x00};
	uint8_t rx[sizeof(tx)];
	for(int i=0; i<sizeof(rx); i++)
		rx[i] = 0x00;
  
	//cspin->write(0);
	if(spi->transfer(tx,0,sizeof(tx))!=mraa::SUCCESS)
		printf("ERROR IN SPIreadOneRegister\n");
	//cspin->write(1);

	return rx[2];
}

void ADXL362::SPIwriteOneRegister(uint8_t regAddress, uint8_t regValue){
  
	//cspin->write(0);
	uint8_t tx[] = {0x0A,regAddress,regValue};
	if(spi->transfer(tx,0,sizeof(tx))!=mraa::SUCCESS)
		printf("ERROR IN SPIwriteOneRegister\n");
	//cspin->write(1);
}

int16_t ADXL362::SPIreadTwoRegisters(uint8_t regAddress){
	int16_t twoRegValue = 0;

	uint8_t tx[] = {0x0B,regAddress,0x00,0x00};
	uint8_t rx[sizeof(tx)];
	for(int i=0; i<sizeof(rx); i++)
		rx[i] = 0x00;
  
	//cspin->write(0);
	if(spi->transfer(tx,rx,sizeof(tx))!=mraa::SUCCESS)
		printf("ERROR IN SPIreadTwoRegisters\n");
	//cspin->write(1);

	return (tx[3]<<8)|tx[2];
}  

void ADXL362::SPIwriteTwoRegisters(uint8_t regAddress, int16_t twoRegValue){
	uint8_t twoRegValueH = twoRegValue >> 8;
	uint8_t twoRegValueL = twoRegValue;
	uint8_t tx[] = {0x0A,regAddress,twoRegValueL,twoRegValueH};
  
	//cspin->write(0);
	if(spi->transfer(tx,0,sizeof(tx))!=mraa::SUCCESS)
		printf("ERROR IN SPIwriteTwoRegisters\n");
	//cspin->write(1);
}