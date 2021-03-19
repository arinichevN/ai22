#ifndef DEVICE_AOID_H
#define DEVICE_AOID_H

#include "../../lib/common.h"

typedef struct {
	Aoid main;
	Aoid pin;
	Aoid poll_interval;
} DeviceAoid;

extern void deviceAoid_build(DeviceAoid *self, Aoid *next, Aoid *parent, size_t *id, void *vdevice);

#endif 
