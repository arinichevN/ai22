#ifndef APP_SENSOR_MUTEX_H
#define APP_SENSOR_MUTEX_H

#include "../../lib/dstructure.h"
#include "../../lib/serial.h"
#include "../../lib/common.h"
/*
 It takes about 4ms to read from DHT22 sensor
 in case we have many connected sensors, it may appear many sensors to 
 read one after another within one cycle, so cycle duration will be more
 than 4ms. We should limit cycle duration to 4ms to make our application
 more responsive. We will use mutex with time delay to achieve this. 
 Mutex will put sensors in queue and will apply certain delay between sensors. 
 During this delay other tasks will run.
*/

extern void sensorMutex_begin();

extern int sensorMutex_lock(MutexClient *client);

extern void sensorMutex_unlock(MutexClient *client);

#endif 
