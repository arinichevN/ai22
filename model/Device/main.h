#ifndef DEVICE_H
#define DEVICE_H

#include "../../lib/dstructure.h"
#include "../../lib/common.h"
#include "../../lib/ton.h"
#include "../../lib/acp/main.h"
#include "../../lib/acp/loop/main.h"
#include "../../lib/acp/loop/server/main.h"
#include "../../lib/acp/command/main.h"

#include "../../app/sensorMutex/main.h"

#ifdef USE_AOIDS
#include "../../lib/aoid.h"
#include "../../app/serials/server/aoid.h"
#include "aoid.h"
#endif

#include "../../pmem/main.h"
#include "../interface/iMaster.h"
#include "../interface/iSlave.h"
#include "../Channel/main.h"
#include "../Dht22/main.h"
#include "param.h"

typedef struct device_st Device;
struct device_st {
	int pin;
	Channel temperature;
	Channel humidity;
	dk_t device_kind;
	Dht22 sensor;
	Ton tmr;
	MutexClient sensor_mutex_client;
	iMaster im_master;
	iSlave *slave_t;
	iSlave *slave_h;
#ifdef USE_AOIDS
	DeviceAoid aoid;
#endif
	void (*control)(Device *);
	err_t error_id;
	size_t ind;
	Device *next;
};


extern state_t device_getState(Device *self);
extern err_t device_getError(Channel *self);
extern void device_begin(Device *self, size_t ind);
extern int device_start(Device *self);
extern int device_stop(Device *self);
extern int device_disconnect(Device *self);
extern int device_reset(Device *self);
extern void device_free(Device *self);


#ifdef USE_AOIDS
extern void device_buildAoids(Device *self, Aoid *next_oid, Aoid *parent_oid, size_t *id);
#endif

#ifdef USE_NOIDS
extern Noid *device_buildNoids(Device *self, Noid *prev);
#endif

#endif 
