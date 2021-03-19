#include "param.h"

err_t deviceParam_check(const DeviceParam *self){
	err_t r = channelParam_check(&self->temperature);
	if(r != ERROR_NO){
		return r;
	}
	r = channelParam_check(&self->humidity);
	if(r != ERROR_NO){
		return r;
	}
	return ERROR_NO;
}
