#include "main.h"

/*
 * -user_config:
 * put default parameters for each device in this array
 * DeviceParam structure members:
 * pin,
 * poll interval (milliseconds, > 2000),
 * {temperature channel id},
 * {humidity channel id}
 */
const DeviceParam DEVICE_DEFAULT_PARAMS[] = {
	{3, 2300, {31}, {32}},
	{4, 2300, {33}, {34}},
	{5, 2300, {35}, {36}},
};

#define DEVICE_COUNT (sizeof DEVICE_DEFAULT_PARAMS / sizeof DEVICE_DEFAULT_PARAMS[0])

Device device_buf[DEVICE_COUNT];

DeviceLList devices = LLIST_INITIALIZER;

int devices_activeExists(){
	return deviceLList_activeExists(&devices);
}

int devices_getIdFirst(int *out){
	return deviceLList_getIdFirst(&devices, out);
}

void devices_begin(int default_btn){
	LLIST_BUILD_FROM_ARRAY_N(devices, device_buf, DEVICE_COUNT)
	size_t ind = 0;
	if(default_btn == BUTTON_DOWN){
		FOREACH_DEVICE {
			pmem_saveDeviceParam(&DEVICE_DEFAULT_PARAMS[ind], ind);
			printd("default param has been saved for device ind \n"); printdln(ind);
			ind++;
		}
	}
	ind = 0;
	FOREACH_DEVICE {
		device_begin(device, ind); 
		ind++;
	}
}

#ifdef USE_AOIDS
void devices_buildAoids(Aoid *next_oid, Aoid *parent_oid, size_t *id){
	return deviceLList_buildAoids(&devices, next_oid, parent_oid, id);
}
#endif

#ifdef USE_NOIDS
Noid *devices_buildNoids(Noid *prev){
	return deviceLList_buildNoids(&devices, prev);
}
#endif
