#ifndef CHANNEL_H
#define CHANNEL_H

#include "../../lib/dstructure.h"
#include "../../lib/common.h"
#include "../../lib/ton.h"
#include "../../lib/acp/main.h"
#include "../../lib/acp/loop/main.h"
#include "../../lib/acp/loop/server/main.h"
#include "../../lib/acp/command/main.h"

#ifdef USE_AOIDS
#include "../../lib/aoid.h"
#include "../../app/serials/server/aoid.h"
#include "aoid.h"
#endif
#ifdef USE_NOIDS
#include "../../lib/noid.h"
#include "../../app/serials/server/noid.h"
#endif
#include "../../pmem/main.h"
#include "../interface/iMaster.h"
#include "../interface/iSlave.h"
#include "param.h"

typedef struct channel_st Channel;

struct channel_st {
	int id;
	Fts output;
	dk_t device_kind;
	
	state_t state;
	iSlave im_slave;
	iMaster *master;
#ifdef USE_AOIDS
	ChannelAoid aoid;
#endif
#ifdef USE_NOIDS
	Noid noid;
#endif
	err_t error_id;
};


extern state_t channel_getState(Channel *self);
extern err_t channel_getError(Channel *self);
extern void channel_begin(Channel *self, size_t ind);
extern int channel_start(Channel *self);
extern int channel_stop(Channel *self);
extern int channel_disconnect(Channel *self);
extern err_t channel_setParam(Channel *self, const ChannelParam *param);

#ifdef USE_AOIDS
extern void channel_buildAoids(Channel *self, Aoid *next_oid, Aoid *parent_oid, size_t *id);
#endif

#ifdef USE_NOIDS
extern Noid *channel_buildNoids(Channel *self, Noid *prev);
#endif

#endif 
