#ifndef PMEM_H
#define PMEM_H

#include <EEPROM.h>
#include "../model/channel.h"
#include "../app/main.h"
#include "../app.h"
#include "../util/crc.h"

typedef struct {
	int id;
	int device_kind;
	unsigned long poll_interval;
	int enable;
} PmemChannel;

extern int pmem_getPChannel(PmemChannel *item, size_t ind) ;

extern int pmem_getPChannelForce(PmemChannel *item, size_t ind) ;

extern int pmem_getAppConfig(AppConfig *item) ;

extern int pmem_getAppConfigForce(AppConfig *item);

extern int pmem_getChannel(Channel *item, size_t ind);

extern int pmem_saveChannel(Channel *item, size_t ind);

extern int pmem_savePChannel(PmemChannel *item, size_t ind);

extern int pmem_saveAppConfig(AppConfig *item);

#define PMEMCHANNEL_GET_FIELD_FUNC(FIELD) pmcgff ## FIELD
#define PMEMCHANNEL_DEF_GET_FIELD_FUNC(FIELD, T) T PMEMCHANNEL_GET_FIELD_FUNC(FIELD)(PmemChannel *item){return item->FIELD;}

#define PMEMCHANNEL_SET_FIELD_FUNC(FIELD) pmcsff ## FIELD
#define PMEMCHANNEL_DEF_SET_FIELD_FUNC(FIELD, T) void PMEMCHANNEL_SET_FIELD_FUNC(FIELD)(PmemChannel *item, T v){item->FIELD = v;}

#endif 
