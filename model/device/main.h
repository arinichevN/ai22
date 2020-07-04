#ifndef MODEL_DEVICE_MAIN_H
#define MODEL_DEVICE_MAIN_H

#include "../../util/common.h"
#include "../../util/ton.h"
#include "dht22.h"
#include "../channel.h"

/*
 * -user_config:
 * set number of DHT22 sensors you want to use:
 */
#define DEVICE_COUNT 3

#define FOREACH_DEVICE(LIST) for(size_t i=0;i<DEVICE_COUNT;i++){ Device *device = &LIST[i];
	
typedef struct {
	int pin;
	int (*read_func)(int, double*, double*);
	int kind;
	FTS out1;
	FTS out2;
	Ton tmr;
	struct channel_st *channel1;
	struct channel_st *channel2;
	int state;
} Device;

extern void device_begin(Device *item);
extern void device_start(Device *item);
extern void device_stop(Device *item);
extern void device_control(Device *item);

#endif 
