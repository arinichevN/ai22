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
	int state;
	size_t ind;
	struct channel_st *next;
};
typedef struct channel_st Channel;

DEC_LLIST(Channel)

#define FOREACH_CHANNEL(LIST) FOREACH_LLIST(channel, LIST, Channel){

extern void channel_setDeviceKind(Channel *item, int kind);
extern const char *channel_getStateStr(Channel *item);
extern const char *channel_getErrorStr(Channel *item);
extern void channel_setParam(Channel *item, int id, unsigned long poll_period_ms, int cs, int sclk, int miso);
extern void channel_begin(Channel *item);
extern int channel_start(Channel *item);
extern int channel_activate(Channel *item);
extern void channel_deviceFailed(Channel *item);
extern int channel_stop(Channel *item);
extern int channel_control(Channel *item);
extern int channels_getIdFirst(ChannelLList *channels, int *out);
extern int channel_getDeviceKind(Channel *item);
extern unsigned long channel_getPollInterval(Channel *item);

#endif 
