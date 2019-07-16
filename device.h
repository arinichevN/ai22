#ifndef DEVICE_H
#define DEVICE_H

#include "common.h"
#include "ton.h"
#include "puart.h"

struct Channel{
	int id;
	FTS output;
    int selected;
};

struct Device{
	int pin;
	Channel humidity;
	Channel temperature;
	Ton tmr;
	int state;
};

extern void device_init(Device *item, int tid, int hid, int interval_ms, int pin);

extern int device_control(Device *item);

#endif 
