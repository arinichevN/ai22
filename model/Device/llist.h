#ifndef DEVICE_LLIST_H
#define DEVICE_LLIST_H

#ifdef USE_AOIDS
#include "../../lib/aoid.h"
#include "../../app/serials/server/aoid.h"
#endif
#include "main.h"

typedef struct {
	Device *top; 
	Device *last; 
	size_t length;
#ifdef USE_AOIDS
	Aoid aoid;
#endif
} DeviceLList;

extern int deviceLList_activeExists(DeviceLList *self);

extern int deviceLList_getIdFirst(DeviceLList *self, int *out);

#ifdef USE_AOIDS
void deviceLList_buildAoids(DeviceLList *self, Aoid *next_oid, Aoid *parent_oid, size_t *id);
#endif

#ifdef USE_NOIDS
extern Noid *deviceLList_buildNoids(DeviceLList *self, Noid *prev);
#endif

#endif 
