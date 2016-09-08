import time, sys, signal, atexit, mraa, time, datetime, csv
import numpy as np
import matplotlib.pyplot as plt
import pyupm_ds1307 as upmDs1307

millis = lambda: int(round(time.time() * 1000))

dataArray = []

def runningMean (insertData, precision):
	dataArray.insert(0, insertData)
	return float(float(sum(dataArray[:precision])) / (float(precision) if len(dataArray) > precision else float(len(dataArray))))

def delay (milliseconds):
	currTime = millis()
	while millis() - currTime < milliseconds:
		pass

class LED:
	def __init__ (self, pin):
		self._pin = mraa.Gpio(pin)
		self._pin.dir(mraa.DIR_OUT)
		self.set(False)
	def set (self, status):
		self._status = status
		self._pin.write(status)
	def on (self):
		self.set(True)
	def off (self):
		self.set(False)
	def toggle (self):
		self.set(~self._status)
	def note (self):
		self.toggle()
		delay(100)
		self.toggle()
		delay(50)
	def rest (self):
		delay(150)
	def mario (self):
		self.off()
		self.note()
		self.note()
		self.rest()
		self.note()
		self.rest()
		self.note()
		self.note()
		self.rest()
		self.note()
		self.rest()
		self.rest()
		self.rest()
		self.note()
	def state (self):
		return self._status

class Button:
	def __init__ (self, pin):
		self._pin = mraa.Gpio(pin)
		self._pin.dir(mraa.DIR_IN)
	def check (self):
		return self._pin.read()

class RTC:
	def __init__ (self, bus):
		self._clock = upmDs1307.DS1307(bus)
	def getTime (self):
		self._result = self._clock.loadTime()
		if not self._result:
			print "rip in pippirini"
			sys.exit(0)
	def timeString (self):
		self.getTime()
		timeStr = "{0}/{1:02d}/{2:02d} {3}:{4:02d}:{5:02d}".format(
		self._clock.month, self._clock.dayOfMonth, self._clock.year + 2000,
		self._clock.hours, self._clock.minutes, self._clock.seconds)
		if (self._clock.amPmMode):
			timeStr += (" PM" if self._clock.pm else " AM")
		return timeStr
	def timeDateTime (self):
		self.getTime()
		return datetime.datetime(self._clock.year + 2000, self._clock.month, self._clock.dayOfMonth, self._clock.hours + (12 if self._clock.pm else 0), self._clock.minutes, self._clock.seconds)
	def timeUnix (self):
		self.getTime()
		return int(datetime.datetime(self._clock.year + 2000, self._clock.month, self._clock.dayOfMonth, self._clock.hours + (12 if self._clock.pm else 0), self._clock.minutes, self._clock.seconds).strftime("%s"))
	def setTime (self, month, dayOfMonth, year, hours, minutes, seconds, amPmMode, pm):
		self._clock.month = month
		self._clock.dayOfMonth = dayOfMonth
		self._clock.year = year
		self._clock.hours = hours
		self._clock.minutes = minutes
		self._clock.seconds = seconds
		self._clock.amPmMode = amPmMode
		self._clock.pm = pm
		self._clock.setTime()

class Shield:
	triggerPinNum = 2
	def __init__ (self, port, baud):
		self.baud = baud
		self.port = port
		self.Serial = mraa.Uart(port)
		self.Serial.setBaudRate(baud)
		self.Serial.setFlowcontrol(False, False)
		self.triggerPin = mraa.Gpio(self.triggerPinNum)
		self.triggerPin.dir(mraa.DIR_OUT)
	def powerOn (self):
		# if self.sendCommand("AT", "OK", 2) == 0:
			self.triggerPin.write(1)
			delay(1)
			self.triggerPin.write(0)
			# self.sendCommand("AT", "OK", 2)
	def println (self, message):
		message = message + "\r\n"
		self.Serial.writeStr(message)
	def write (self, message):
		self.Serial.writeStr(message)
	def waitForCommand (self):
		delay(1)
		sendMessage = raw_input("Command: ")
		self.println(sendMessage)
	def waitForReply (self, timeSec):
		currTime = int(time.time())
		message = ""
		# self.Serial.flush()
		while True:
			if self.Serial.dataAvailable():
				char = self.Serial.readStr(1)
				# print message
				if char == "\n" or char == "\r":
					if message == "":
						pass
					else:
						return message
				else:
					message = message + char
			elif int(time.time()) - currTime > timeSec:
				# print("No response.")
				return -1
	def sendCommand (self, command, expected, timeSec):
		self.println(command)
		if self.waitForReply(timeSec)[4:] == expected:
			return True
		else:
			return False

latchInterval = 500
latchTime = millis()
f = None
writer = None
startTime = None
accessTime = None
tableTime = []
tableDist = []

Green = LED(9)
clock = RTC(0)
switch = Button(8)
Serial = Shield(0, 9600)
Serial.powerOn()

print clock.timeString()
print clock.timeUnix()
sys.exit()

lastCheck = switch.check()
newCheck = switch.check()

Green.mario()
print "Ready."

while True:
	if lastCheck == False and newCheck == True and millis() - latchTime > latchInterval:
		latchTime = millis()
		Green.toggle()
		if Green.state():
			accessTime = clock.timeUnix()
			f = open("/media/card/distance/log-{}.csv".format(accessTime), "w+")
			writer = csv.writer(f)
			writer.writerow(("Timestamp", "Distance (in cm)"))

			initialResult = Serial.waitForReply(1)
			while (initialResult == -1):
				Serial.println("")
				initialResult = Serial.waitForReply(1)
				print "boo"

			startTime = millis()
			tableTime = []
			tableDist = []
			dataArray = []

		else:
			f.close()
			plt.figure()
			plt.plot(np.array(tableTime), np.array(tableDist), 'k')
			plt.title('Distance vs. Time last {}'.format(clock.timeString()))
			plt.xlabel('Time (seconds)')
			plt.ylabel('Distance (centimeters)')
			plt.grid(which='both', axis='both')
			plt.savefig("/media/card/distance/graph-distance-{}.png".format(accessTime))
			plt.close()
	if Green.state():
		state = Serial.waitForReply(1)
		if (state == -1):
			sys.exit(0)
		if float(state) > 3000.00:
			continue
		smooth = runningMean(float(state), 15)
		print "{},{}".format(clock.timeString(), smooth)
		tableTime.append((millis() - startTime) / 1000.0)
		tableDist.append(smooth)
		writer.writerow((clock.timeString(), smooth))

	lastCheck = newCheck
	newCheck = switch.check()
	Green.mario()
