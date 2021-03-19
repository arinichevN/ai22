#include "main.h"

Rtmutex sensor_mutex_n;
Rtmutex *sensor_mutex = &sensor_mutex_n;

void sensorMutex_begin(){
	rtmutex_begin(sensor_mutex, 5);
}

int sensorMutex_lock(MutexClient *client){
	return rtmutex_lock(sensor_mutex, client);
}

void sensorMutex_unlock(MutexClient *client){
	return rtmutex_unlock(sensor_mutex, client);
}
