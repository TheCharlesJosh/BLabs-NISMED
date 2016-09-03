CFLAGS = -lmraa -std=c++11
OBJECTS = RTClib.o temp_sensor.o cond_probe.o analog_pin.o ADXL362.o ph_sensor.o csv_export.o lcd_ui.o
TARGET = temp1

default:
	g++ $(OBJECTS) $(TARGET).cpp -o $(TARGET) $(CFLAGS)

make objects: RTClib temp_sensor cond_probe analog_pin ADXL362 ph_sensor csv_export lcd_ui

RTClib: RTClib.cpp
	g++ -c RTClib.cpp $(CFLAGS)

temp_sensor: temp_sensor.cpp
	g++ -c temp_sensor.cpp $(CFLAGS)

cond_probe: cond_probe.cpp
	g++ -c cond_probe.cpp $(CFLAGS)

analog_pin: analog_pin.cpp
	g++ -c analog_pin.cpp $(CFLAGS)

ADXL362: ADXL362.cpp
	g++ -c ADXL362.cpp $(CFLAGS)

ph_sensor: ph_sensor.cpp
	g++ -c ph_sensor.cpp $(CFLAGS)

csv_export: csv_export.cpp
	g++ -c csv_export.cpp $(CFLAGS)

lcd_ui: lcd_ui.cpp
	g++ -c lcdui.cpp $(CFLAGS)