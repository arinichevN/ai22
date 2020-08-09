#include "channel.h"

Channel channel_buf[CHANNEL_COUNT];

int channel_normalizeDeviceKind(int kind){
	switch(kind){
		case DEVICE_KIND_DHT22T:
		case DEVICE_KIND_DHT22H:
			return kind;
	}
	printdln("WARNING: unknown device");
	return DEVICE_KIND_UNKNOWN;
}

void channel_setDeviceKind(Channel *item, int kind){
	item->device_kind = kind;
}

int channel_check(Channel *item){
	if(!common_checkBlockStatus(item->enable)){
		return ERROR_BLOCK_STATUS;
	}
	int kind = channel_normalizeDeviceKind(item->device_kind);
	if(kind == DEVICE_KIND_UNKNOWN){
		return ERROR_DEVICE_KIND;
	}
    return ERROR_NO;
}

const char *channel_getStateStr(Channel *item){
	switch(item->state){
		case RUN: return "RUN";
		case OFF: return "OFF";
		case FAILURE: return "FAILURE";
		case DISABLE: return "DISABLE";
		case INIT: return "INIT";
	}
	return "?";
}

const char *channel_getErrorStr(Channel *item){
	return getErrorStr(item->error_id);
}

void channel_setDefaults(Channel *item, size_t ind){
	if(ind % 2 == 0){
		item->device_kind = DEFAULT_DEVICE_KIND + 1;
	}else{
		item->device_kind = DEFAULT_DEVICE_KIND + 2;
	}
	item->poll_interval = DEFAULT_CHANNEL_POLL_INTERVAL_MS;
	item->id = ind + DEFAULT_CHANNEL_FIRST_ID;
	item->enable = DEFAULT_CHANNEL_ENABLE;
}

static void channel_setFromNVRAM(Channel *item, size_t ind){
	if(!pmem_getChannel(item, ind)){
		printdln("   failed to get channel");
		item->error_id = ERROR_PMEM_READ;
		return;
	}
}

void channel_begin(Channel *item, size_t ind, int default_btn){
	printd("beginning channel ");printd(ind); printdln(":");
	if(default_btn == BUTTON_DOWN){
		channel_setDefaults(item, ind);
		pmem_saveChannel(item, ind);
		printd("\tdefault param\n");
	}else{
		channel_setFromNVRAM(item, ind);
		printd("\tNVRAM param\n");
	}
	item->ind = ind;
	item->state = INIT;
	printd("\tid: ");printdln(item->id);
	printd("\n");
}

void channels_coopDevices(Channel channels[], Device devices[]){
	FOREACH_DEVICE(devices)
		Channel *channel1 = &channels[2*i];
		Channel *channel2 = &channels[2*i+1];
		device->channel1 = channel1;
		device->channel2 = channel2;
		channel1->device = device;
		channel2->device = device;
	}
}

void channels_buildFromArray(ChannelLList *channels, Channel arr[]){
	if(CHANNEL_COUNT <= 0) return;
	channels->length = CHANNEL_COUNT;
	channels->top = &arr[0];
	channels->last = &arr[CHANNEL_COUNT - 1];
	for(size_t i = 0;i<CHANNEL_COUNT;i++){
		arr[i].next = NULL;
	}
	for(size_t i = 0; i<CHANNEL_COUNT-1; i++){
		arr[i].next = &arr[i+1];
	}
}


int channels_begin(ChannelLList *channels, int default_btn){
	extern Channel channel_buf[];
	channels_buildFromArray(channels, channel_buf);
	size_t i = 0;
	FOREACH_CHANNEL(channels)
		channel_begin(channel, i, default_btn); i++;
		if(channel->error_id != ERROR_NO) return 0;
	}
	return 1;
}

int channel_start(Channel *item){
	printd("starting channel ");printd(item->ind);printdln(":");
	item->enable = YES;
	item->state = INIT;
	PmemChannel pchannel;
	if(pmem_getPChannel(&pchannel, item->ind)){
		pchannel.enable = item->enable;
		pmem_savePChannel(&pchannel, item->ind);
	}
	return 1;
}

int channel_stop(Channel *item){
	printd("stopping channel ");printdln(item->ind); 
	item->enable = NO;
	item->state = INIT;
	PmemChannel pchannel;
	if(pmem_getPChannel(&pchannel, item->ind)){
		pchannel.enable = item->enable;
		pmem_savePChannel(&pchannel, item->ind);
	}
	return 1;
}

int channel_reload(Channel *item){
	printd("reloading channel ");printd(item->ind); printdln(":");
	channel_setFromNVRAM(item, item->ind);
	item->state = INIT;
	return 1;
}

int channel_activate(Channel *item){
	item->state = RUN;
	return 1;
}

void channel_deviceFailed(Channel *item){
	item->state = FAILURE;
	item->error_id = ERROR_DEVICE_KIND;
}

int channels_activeExists(ChannelLList *channels){
	FOREACH_CHANNEL(channels)
		if(channel->state != OFF){
			return 1;
		}
	}
	return 0;
}

void channels_stop(ChannelLList *channels){
	FOREACH_CHANNEL(channels)
		channel_stop(channel);
	}
}

int channels_getIdFirst(ChannelLList *channels, int *out){
	int success = 0;
	int f = 0;
	int v;
	FOREACH_CHANNEL(channels)
		if(!f) { v=channel->id; f=1; success=1;}
		if(channel->id < v) v = channel->id;
	}
	*out = v;
	return success;
}


static int channel_selectDeviceOutput(Channel *item){
	switch(item->device_kind){
		case DEVICE_KIND_DHT22T:
			item->out = &item->device->out1;
			return 1;
		case DEVICE_KIND_DHT22H:
			item->out = &item->device->out2;
			return 1;
	}
	return 0;
}

int channel_control(Channel *item){
	switch(item->state){
		case RUN:
			item->error_id = ERROR_NO;
			if(item->out->state != 1){
				item->error_id = ERROR_READ;
			}
			break;
		case OFF:
			break;
		case FAILURE:
			break;
		case INIT:
			item->error_id = ERROR_NO;
			item->error_id = channel_check(item);
		    if(item->error_id != ERROR_NO){
		        item->state = FAILURE;
		        break;
		    }
		    if(!channel_selectDeviceOutput(item)){
				item->error_id = ERROR_DEVICE_KIND;
				item->state = FAILURE;
				break;
			}
			item->state = OFF;
			if(item->enable == YES){
				device_start(item->device);
			}else{
				device_stop(item->device);
			}
			break;
		default:
			break;
	}
	return item->state;
}



int CHANNEL_FUN_GET(enable)(Channel *item){return item->enable;}
int CHANNEL_FUN_GET(device_kind)(Channel *item){return item->device_kind;}
unsigned long CHANNEL_FUN_GET(poll_interval)(Channel *item){return item->poll_interval;}
