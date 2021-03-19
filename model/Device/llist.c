#include "llist.h"

int deviceLList_activeExists(DeviceLList *self){
	FOREACH_LLIST(device, self, Device){
		if(device->control != device_OFF){
			return 1;
		}
	}
	return 0;
}

int deviceLList_getIdFirst(DeviceLList *self, int *out){
	int success = 0;
	int f = 0;
	int v;
	FOREACH_LLIST(device, self, Device){
		if(!f) {v=device->temperature.id; f=1; success=1;}
		if(device->temperature.id < v) v = device->temperature.id;
		if(device->humidity.id < v) v = device->humidity.id;
	}
	*out = v;
	return success;
}

#ifdef USE_AOIDS
static void deviceLList_serveAoidRequestSelf(void *vself, Aoid *oid, void *vserver, int command){
	//DeviceLList *self = (DeviceLList *) vself;
	Acpls *server = (Acpls *) vserver;
	switch(command){
		case CMD_AOID_GET_ACP_COMMAND_SUPPORTED:
			aoidServer_sendSupportedNone(oid, server);
			return;
	}
	acpls_reset(server);
}

void deviceLList_buildAoids(DeviceLList *self, Aoid *next_oid, Aoid *parent_oid, size_t *id){
	Aoid *next_oid_l = next_oid;
	if(self->top != NULL){
		next_oid_l = &self->top->aoid.main;
	}
	//AOID_SET_PARAM(OID, 			NEXT,			PARENT,			KIND,				DESCR,							FUNCTION, 							DATA,	ID)
	AOID_SET_PARAM(&self->aoid,		next_oid_l,		parent_oid,		AOID_KIND_LIST,		AOID_DESCRIPTION_DEVICES,		deviceLList_serveAoidRequestSelf,	self,	*id)	
	FOREACH_LLIST(item, self, Device){
		next_oid_l = next_oid;
		if(item->next != NULL){
			next_oid_l = &item->next->aoid.main;
		}
		device_buildAoids(item, next_oid_l, &self->aoid, id);
	}
}
#endif

#ifdef USE_NOIDS
Noid *deviceLList_buildNoids(DeviceLList *self, Noid *prev){
	Noid *last_oid = NULL;
	Noid *prev_oid = prev;
	FOREACH_LLIST(item, self, Device){
		last_oid = device_buildNoids(item, prev_oid);
		prev_oid = last_oid;
	}
	return last_oid;
}
#endif
