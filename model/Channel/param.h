#ifndef CHANNEL_PARAM_H
#define CHANNEL_PARAM_H

#include "../../lib/common.h"

typedef struct {
	int id;
} ChannelParam;

extern err_t channelParam_check(const ChannelParam *self);

#endif 
