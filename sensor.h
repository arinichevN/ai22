#ifndef SENSOR_DS18B20_H
#define SENSOR_DS18B20_H

#include "ton.h"
#include "common.h"

struct Sensor {
	int ind;
	FTS output;
	Ton tmr;
	int state;
};

extern void sensor_init(Sensor *item, int pin);

extern int sensor_control(Sensor *item);

#endif 
