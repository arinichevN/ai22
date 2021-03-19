#include "main.h"

#define DHT22_MAXTIMINGS 80
#define DHT22_MAX_VAL 500 //this limit depends on your chip frequency

#define DHT22_WAITPULSE(level) \
			count = 0;\
			while (DIRECT_READ(reg, mask) == level) {\
				if (count >= DHT22_MAX_VAL) {\
					printd("timeout "); printd(" count="); printdln(count);\
					interrupts();\
					self->control = dht22_READ1;\
					return STATE_FAILURE;\
				}\
				count++;\
			}

static int dht22_READ1(Dht22 *self, int pin, double *temperature, double *humidity);
static int dht22_READ2(Dht22 *self, int pin, double *temperature, double *humidity);

static int dht22_READ1(Dht22 *self, int pin, double *temperature, double *humidity){
	IO_REG_TYPE mask IO_REG_MASK_ATTR = PIN_TO_BITMASK(pin);
	volatile IO_REG_TYPE *reg IO_REG_BASE_ATTR = PIN_TO_BASEREG(pin);
	DIRECT_MODE_OUTPUT(reg, mask);
	DIRECT_WRITE_LOW(reg, mask);
	tonu_reset(&self->tmr);
	self->control = dht22_READ2;
	return STATE_BUSY;
}


static int dht22_READ2(Dht22 *self, int pin, double *temperature, double *humidity){
	if(!tonu(&self->tmr)){
		return STATE_BUSY;
	}
	noInterrupts();
	IO_REG_TYPE mask IO_REG_MASK_ATTR = PIN_TO_BITMASK(pin);
	volatile IO_REG_TYPE *reg IO_REG_BASE_ATTR = PIN_TO_BASEREG(pin);
	DIRECT_MODE_INPUT(reg, mask);
	int count;
	DHT22_WAITPULSE(HIGH)
	DHT22_WAITPULSE(LOW)
	DHT22_WAITPULSE(HIGH)

	int cycles[DHT22_MAXTIMINGS];
	for (int i = 0; i < DHT22_MAXTIMINGS; i += 2) {
		DHT22_WAITPULSE(LOW)
		cycles[i] = count;
		DHT22_WAITPULSE(HIGH)
		cycles[i + 1] = count;
	}
	interrupts();
	//for (int i = 0; i < DHT22_MAXTIMINGS; i += 2) {
		//printd(cycles[i]); printd("-"); printd(cycles[i+1]); printd(" ");
	//}
	//printdln(" ");
	uint8_t data[5] = {0, 0, 0, 0, 0};
	for (int i = 0; i < 40; ++i) {
		int lowCycles  = cycles[2 * i];
		int highCycles = cycles[2 * i + 1];
		data[i / 8] <<= 1;
		if (highCycles > lowCycles) {
		  data[i / 8] |= 1;
		}
	}
	if (data[4] == ((data[0] + data[1] + data[2] + data[3]) & 0xFF)) {
		double h = (double) data[0] * 256 + (double) data[1];
		h /= 10;
		double t = (double) (data[2] & 0x7F) * 256 + (double) data[3];
		t /= 10.0;
		if ((data[2] & 0x80) != 0) {
			t *= -1;
		}
		*temperature = t;
		*humidity = h;
		pinMode(pin, INPUT);
		self->control = dht22_READ1;
		return STATE_DONE;
	}  else {
		printd("pin: ");printd(pin); printd(": ");printd("bad data crc\n");
		pinMode(pin, INPUT);
		self->control = dht22_READ1;
		return STATE_FAILURE;
	}
}

void dht22_begin(Dht22 *self, int pin){
	pinMode(pin, INPUT);
	ton_setInterval(&self->tmr, 1000);
	self->control = dht22_READ1;
}
