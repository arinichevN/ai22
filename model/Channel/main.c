#include "main.h"

static void channel_setOutput(void *vself, double v, struct timespec tm){
	Channel *self = (Channel *) vself;
	self->output.value = v;
	self->output.tm = tm;
	self->output.success = YES;
	self->error_id = ERROR_NO;
}
static void channel_setOutputDummy(void *vself, double v, struct timespec tm){
	;
}

static void channel_outputFailed(void *vself){
	Channel *self = (Channel *) vself;
	self->output.success = NO;
	self->error_id = ERROR_READ;
}

static void channel_outputFailedDummy(void *vself){
	;
}

static void channel_startByMaster(void *vself){
	Channel *self = (Channel *) vself;
	self->output.value = 0.0;
	self->output.tm = getCurrentTs();
	self->output.success = NO;
	self->im_slave.setOutput = channel_setOutput;
	self->im_slave.outputFailed = channel_outputFailed;
	self->state = STATE_RUN;
}

static void channel_stopByMaster(void *vself){
	Channel *self = (Channel *) vself;
	self->output.success = NO;
	self->error_id = ERROR_NO;
	self->im_slave.setOutput = channel_setOutputDummy;
	self->im_slave.outputFailed = channel_outputFailedDummy;
	self->state = STATE_OFF;
}

state_t channel_getState(Channel *self){
	return self->state;
}

err_t channel_getError(Channel *self){
	return self->error_id;
}

static void channel_buildInterfaces(Channel *self, Channel *other_channel){
	iSlave *ifc = &self->im_slave;
	ifc->self = self;
	ifc->state = &self->state;
	ifc->other = &other_channel->im_slave;
	ifc->start = channel_startByMaster;
	ifc->stop = channel_stopByMaster;
	ifc->setOutput = channel_setOutputDummy;
	ifc->outputFailed = channel_outputFailedDummy;
}

err_t channel_setParam(Channel *self, const ChannelParam *param){
	self->id = param->id;
	printd("channel param: id "); printdln(self->id);
	return ERROR_NO;
}

void channel_begin(Channel *self, dk_t device_kind, Channel *other_channel, iMaster *master){
	channel_buildInterfaces(self, other_channel);
	self->device_kind = device_kind;
	self->master = master;
	self->output.value = 0.0;
	self->output.tm = (struct timespec) {0, 0};
	self->output.success = NO;
	self->error_id = ERROR_NO;
	self->state = STATE_OFF;
}

int channel_start(Channel *self){
	if(self->state == STATE_RUN) return 0;
	return self->master->start(self->master->self, &self->im_slave);
}

int channel_stop(Channel *self){
	if(self->state == STATE_RUN){
		return self->master->stop(self->master->self, &self->im_slave);
	}
	return 0;
}

int channel_disconnect(Channel *self){
	return channel_stop(self);
}

#ifdef USE_AOIDS
#include "aoid.c"

void channel_buildAoids(Channel *self, Aoid *next_oid, Aoid *parent_oid, size_t *id){
	channelAoid_build(&self->aoid, next_oid, parent_oid, self, id);
}

#endif

#ifdef USE_NOIDS
#include "noid.c"
#endif

