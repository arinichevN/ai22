#ifndef DHT22_H
#define DHT22_H

#include "../../lib/dstructure.h"
#include "../../lib/common.h"
#include "../../lib/ton.h"
#include "direct_gpio.h"
#include "direct_regtype.h"

typedef struct dht22_st Dht22;
struct dht22_st {
	Ton tmr;
	int (*control)(Dht22 *, int, double *, double *);
};

extern void dht22_begin(Dht22 *self, int pin);
#define dht22_read(self, pin, temperature, humidity) (self)->control((self), pin, temperature, humidity)  

#endif 
