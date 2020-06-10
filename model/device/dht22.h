#ifndef MODEL_DEVICE_DHT22_H
#define MODEL_DEVICE_DHT22_H

#include "../../util/debug.h"
#include "../../util/common.h"

extern int dht22_read(int pin, double *t, double *h);

#endif 
