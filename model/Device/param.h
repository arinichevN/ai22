#ifndef DEVICE_PARAM_H
#define DEVICE_PARAM_H

#include "../../lib/common.h"
#include "../Channel/param.h"

typedef struct {
	int pin;
	unsigned long poll_interval_ms;
	ChannelParam temperature;
	ChannelParam humidity;
} DeviceParam;

extern err_t deviceParam_check(const DeviceParam *self);

#endif 
