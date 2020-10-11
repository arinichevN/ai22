#ifndef MODEL_CHANNEL_H
#define MODEL_CHANNEL_H

#include "../util/dstructure.h"
#include "../util/common.h"
#include "../util/ton.h"
#include "../app/serial.h"
#include "device/main.h"

#define CHANNEL_COUNT DEVICE_COUNT * 2

/*
 * -user_config:
 * here you can set some default parameters for
 * channels
 */
#define DEFAULT_DEVICE_KIND					DEVICE_KIND_DHT22
#define DEFAULT_CHANNEL_FIRST_ID			1
#define DEFAULT_CHANNEL_ENABLE				YES
#define DEFAULT_CHANNEL_POLL_INTERVAL_MS	1000UL

struct channel_st {
	int id;
	int device_kind;
	unsigned long poll_interval;
	Device *device;
	FTS *out;
	int enable;
	int error_id;
	size_t ind;
	void (*control)(struct channel_st *);
	struct channel_st *next;
};
typedef struct channel_st Channel;

DEC_LLIST(Channel)

#define FOREACH_CHANNEL(LIST) FOREACH_LLIST(channel, LIST, Channel)
#define CHANNEL_SAVE_FIELD(F) PmemChannel pchannel;	if(pmem_getPChannel(&pchannel, item->ind)){pchannel.F = item->F; pmem_savePChannel(&pchannel, item->ind);}
#define CHANNEL_FUN_GET(param) channel_get_ ## param

void channel_setDeviceKind(Channel *item, int kind);
extern void channel_deviceFailed(Channel *item);
extern int channel_activate(Channel *item);
extern int channel_getState(Channel *item);
extern void channel_free(Channel *item);
extern int channel_reset(Channel *item);
extern int channel_disconnect(Channel *item);

#endif
