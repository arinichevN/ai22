#include "main.h"

static void device_OFF(Device *self);
static void device_FAILURE(Device *self);
static void device_WAIT(Device *self);
static void device_LOCK(Device *self);
static void device_READ(Device *self);

static int device_masterStart(void *vself, iSlave *slave){
	Device *self = (Device *) vself;
	if(self->control == device_OFF){
		ton_reset(&self->tmr);
		self->control = device_WAIT;
		slave->start(slave->self);
		return 1;
	}
	slave->start(slave->self);
	return 0;
}

static int device_masterStop(void *vself, iSlave *slave){
	Device *self = (Device *) vself;
	slave->stop(slave->self);
	if(*slave->other->state == STATE_OFF){
		device_stop(self);
	}
	return 1;
}

static int device_getChannelParam(void *vself, dk_t device_kind, void *vparam){
	Device *self = (Device *) vself;
	ChannelParam *param = (ChannelParam *) vparam;
	DeviceParam dparam;
	printd("device get param "); printdln(self->ind);
	if(pmem_getDeviceParam(&dparam, self->ind)){
		switch(device_kind){
			case DEVICE_KIND_DHT22T:
				*param = dparam.temperature;
				return 1;
			case DEVICE_KIND_DHT22H:
				*param = dparam.humidity;
				return 1;
			default:
				break;
		}
	}
	return 0;
}

static void device_saveChannelParam(void *vself, dk_t device_kind, void *vparam){
	Device *self = (Device *) vself;
	ChannelParam *param = (ChannelParam *) vparam;
	DeviceParam dparam;
	if(!pmem_getDeviceParam(&dparam, self->ind)){
		return;
	}
	switch(device_kind){
		case DEVICE_KIND_DHT22T:
			dparam.temperature = *param;
			break;
		case DEVICE_KIND_DHT22H:
			dparam.humidity = *param;
			break;
		default:
			return;
	}
	pmem_saveDeviceParam(&dparam, self->ind);
}

state_t device_getState(Device *self){
	if(self->control == device_OFF) {
		return STATE_OFF;
	} else if(self->control == device_WAIT || self->control == device_READ) {
		return STATE_RUN;
	} else if(self->control == device_FAILURE) {
		return STATE_FAILURE;
	}
	return STATE_UNKNOWN;
}

err_t device_getError(Channel *self){
	return self->error_id;
}

static void device_buildIMaster(Device *self, iMaster *ifs){
	ifs->self = self;
	ifs->start = device_masterStart;
	ifs->stop = device_masterStop;
	ifs->getChannelParam = device_getChannelParam;
	ifs->saveChannelParam = device_saveChannelParam;
}

static void device_buildInterfaces(Device *self){
	device_buildIMaster(self, &self->im_master);
}

static err_t device_setParam(Device *self, size_t ind){
	self->ind = ind;
	DeviceParam param;
	if(!pmem_getDeviceParam(&param, ind)){
		printdln("   failed to get device from NVRAM");
		return ERROR_NVRAM_READ;
	}
	err_t r = deviceParam_check(&param);
	
	if(r != ERROR_NO){
		return r;
	}
	self->pin = param.pin;
	ton_setInterval(&self->tmr, param.poll_interval_ms);
	r = channel_setParam(&self->temperature, &param.temperature);
	if(r != ERROR_NO){
		return r;
	}
	r = channel_setParam(&self->humidity, &param.humidity);
	if(r != ERROR_NO){
		return r;
	}
	printd("device param: pin "); printdln(self->pin);
	
	return ERROR_NO;
}

void device_begin(Device *self, size_t ind){
	printd("beginning device ");printd(ind); printdln(":");
	device_buildInterfaces(self);
	mutexcl_reset(&self->sensor_mutex_client);
	self->device_kind = DEVICE_KIND_DHT22;
	self->error_id = device_setParam(self, ind);
	self->control = device_OFF;
	channel_begin(&self->temperature, DEVICE_KIND_DHT22T, &self->humidity, &self->im_master);
	channel_begin(&self->humidity, DEVICE_KIND_DHT22H, &self->temperature, &self->im_master);
	self->slave_t = &self->temperature.im_slave;
	self->slave_h = &self->humidity.im_slave;
	dht22_begin(&self->sensor, self->pin);
	self->control = device_OFF;
	printd("\t ind: "); printd(self->ind); printd(", pin: ");printdln(self->pin);
	printd("\n");
}


void device_free(Device *self){
	;
}

int device_start(Device *self){
	if(self->control == device_OFF){
		printd("starting device ");printd(self->ind);printdln(":");
		self->slave_t->start(self->slave_t->self);
		self->slave_h->start(self->slave_h->self);
		ton_reset(&self->tmr);
		self->control = device_WAIT;
		return 1;
	}
	return 0;
}

int device_stop(Device *self){
	if(self->control == device_FAILURE || self->control == device_OFF){
		return 0;
	}
	printd("stopping device ");printdln(self->ind);
	self->slave_t->stop(self->slave_t->self);
	self->slave_h->stop(self->slave_h->self);
	self->error_id = ERROR_NO;
	self->control = device_OFF;
	return 1;
}

int device_disconnect(Device *self){
	return device_stop(self);
}

int device_reset(Device *self){
	printd("restarting device ");printd(self->ind); printdln(":");
	device_stop(self);
	device_free(self);
	device_begin(self, self->ind);
	return 1;
}

static void device_WAIT(Device *self){
	if(ton(&self->tmr)){
		self->control = device_LOCK;
	}
}

static void device_LOCK(Device *self){
	if(sensorMutex_lock(&self->sensor_mutex_client)){
		self->control = device_READ;
	}
}

static void device_READ(Device *self){
	double t, h;
	int r = dht22_read(&self->sensor, self->pin, &t, &h);
	switch(r){
		case STATE_BUSY:
			return;
		case STATE_DONE:{
				struct timespec tm = getCurrentTs();
				self->slave_t->setOutput(self->slave_t->self, t, tm);
				self->slave_h->setOutput(self->slave_h->self, h, tm);
			}
			sensorMutex_unlock(&self->sensor_mutex_client);
			ton_reset(&self->tmr);
			self->control = device_WAIT;
			return;
	}
	sensorMutex_unlock(&self->sensor_mutex_client);
	self->slave_t->outputFailed(self->slave_t->self);
	self->slave_h->outputFailed(self->slave_h->self);
	ton_reset(&self->tmr);
	self->control = device_WAIT;
	printd("dht22 read failed on pin "); printdln(self->pin);
}

static void device_OFF(Device *self){
	;
}

static void device_FAILURE(Device *self){
	;
}

#ifdef USE_AOIDS
#include "aoid.c"

void device_buildAoids(Device *self, Aoid *next_oid, Aoid *parent_oid, size_t *id){
	deviceAoid_build(&self->aoid, next_oid, parent_oid, self, id);
}

#endif

#ifdef USE_NOIDS

Noid *device_buildNoids(Device *self, Noid *prev){
	Noid *noid1 = channel_buildNoids(&self->temperature, prev);
	Noid *noid2 = channel_buildNoids(&self->humidity, noid1);
	return noid2;
}

#endif
