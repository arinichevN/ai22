#include "main.h"

Device devices[DEVICE_COUNT];

void device_setStaticParam(Device *item, int pin){
	item->pin = pin;
}

void device_begin(Device *item) {
	item->state = OFF;
}

void device_start(Device *item){
	item->state = INIT;
}

void device_stop(Device *item){
	if(item->channel1->state == OFF && item->channel2->state == OFF){
		item->state = OFF;
	}
}


#define DEVICE_IND devi
#define SET_DEVICE_STATIC_PARAM(PIN) if(DEVICE_IND < DEVICE_COUNT){device_setStaticParam(&devices[DEVICE_IND], PIN);DEVICE_IND++;} else {printdln("call SET_DEVICE_STATIC_PARAM for each device (sensor)"); return 0;}
int devices_begin(){
	extern Device devices[];
	size_t DEVICE_IND = 0;
	
	/* 
	 * -user_config:
	 * call
	 * SET_DEVICE_STATIC_PARAM(int pin)
	 * for each channel:
	*/
	//SET_DEVICE_STATIC_PARAM(2);
	SET_DEVICE_STATIC_PARAM(3);
	SET_DEVICE_STATIC_PARAM(7);
	
	if(DEVICE_IND != DEVICE_COUNT){printdln("call SET_DEVICE_STATIC_PARAM for each device (sensor)"); return 0;}
	FOREACH_DEVICE(devices)
		device_begin(device);
	}
	return 1;
}

static int device_selectKind(Device *item){
	if(
	(item->channel1->device_kind == DEVICE_KIND_DHT22T || item->channel1->device_kind == DEVICE_KIND_DHT22H) &&
	(item->channel2->device_kind == DEVICE_KIND_DHT22T || item->channel2->device_kind == DEVICE_KIND_DHT22H)){
		item->kind = DEVICE_KIND_DHT22;
		item->read_func = &dht22_read;
		return 1;
	}
	item->kind = DEVICE_KIND_UNKNOWN;
	item->read_func = NULL;
	return 0;
}

static void device_selectInterval(Device *item){
	if(item->channel1->poll_interval < item->channel2->poll_interval){
		ton_setInterval(&item->tmr, item->channel1->poll_interval);
	}else{
		ton_setInterval(&item->tmr, item->channel2->poll_interval);
	}
}

void device_control(Device *item) {
  switch (item->state) {
    case RUN:
		if (tonr(&item->tmr)) {
			double v1 = 0.0, v2 = 0.0;
			int r = item->read_func(item->pin, &v1, &v2);
			item->out1.state = item->out2.state = r;
			if (r) {
			  item->out1.value = v1;
			  item->out2.value = v2;
			  printd(v1); printd(" "); printdln(v2);
			}
			item->out1.tm = item->out2.tm = getCurrentTs();
		}
      break;
    case OFF:
		break;
	case FAILURE:
		break;
    case INIT:
		device_selectInterval(item);
		ton_reset(&item->tmr);
		item->out1.state = item->out2.state = 0;
		item->out1.tm = item->out2.tm = getCurrentTs();
		if(!device_selectKind(item)){
			channel_deviceFailed(item->channel1);
			channel_deviceFailed(item->channel2);
			item->state = FAILURE;
			return;
		}
		pinMode(item->pin, INPUT);
		channel_activate(item->channel1);
		channel_activate(item->channel2);
		item->state = RUN;
		break;
    default:
      break;
  }
}

