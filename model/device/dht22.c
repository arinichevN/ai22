#include "dht22.h"

#define DHT22_MAXTIMINGS 80
#define DHT22_MAX_VAL 80
#define DHT22_TIMEOUT -1

static int dht22_waitPulse(int pin, int level) {
	int count = 0;
	while (digitalRead(pin) == level) {
		if (count++ >= DHT22_MAX_VAL) {
			return DHT22_TIMEOUT; // Exceeded timeout, fail.
		}
	}
	return count;
}

int dht22_read(int pin, double *t, double *h) {
	pinMode(pin, INPUT);
	delay(1);
	pinMode(pin, OUTPUT);
	digitalWrite(pin, LOW);
	delayMicroseconds(1100); // datasheet says "at least 1ms"
	pinMode(pin, INPUT);
	delayMicroseconds(55);

	if (dht22_waitPulse(pin, LOW) == DHT22_TIMEOUT) {
		printd("pin: ");printd(pin); printd(": ");printdln("failed wl");
		return 0;
	}
	if (dht22_waitPulse(pin, HIGH) == DHT22_TIMEOUT) {
		printd("pin: ");printd(pin); printd(": ");printdln("failed wh");
		return 0;
	}
	
	
	int cycles[DHT22_MAXTIMINGS];
	for (int i = 0; i < DHT22_MAXTIMINGS; i += 2) {
		cycles[i] = dht22_waitPulse(pin, LOW);
		cycles[i + 1] = dht22_waitPulse(pin, HIGH);
	}
	//for (int i = 0; i < DHT22_MAXTIMINGS; i++ ) {
	//	printd(cycles[i]); printd(".");
	//}
	//printdln(" ");
	uint8_t data[5] = {0, 0, 0, 0, 0};
	for (int i = 0; i < 40; ++i) {
		int lowCycles  = cycles[2 * i];
		int highCycles = cycles[2 * i + 1];
		if ((lowCycles == DHT22_TIMEOUT) || (highCycles == DHT22_TIMEOUT)) {
			printd("pin: ");printd(pin); printd(": ");printd("dht22 timeout\n");
			return 0;
		}
		data[i / 8] <<= 1;
		if (highCycles > lowCycles) {
		  data[i / 8] |= 1;
		}
	}


	if (data[4] == ((data[0] + data[1] + data[2] + data[3]) & 0xFF)) {
		*h = ( double ) data[0] * 256 + ( double ) data[1];
		*h /= 10;
		*t = ( double ) ( data[2] & 0x7F ) * 256 + ( double ) data[3];
		*t /= 10.0;
		if ( ( data[2] & 0x80 ) != 0 ) {
			*t *= -1;
		}
		return 1;
	}  else {
		printd("pin: ");printd(pin); printd(": ");printd("bad data crc\n");
		return 0;
	}

}
