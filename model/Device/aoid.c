#include "aoid.h"

static void device_serveAoidRequestSelf(void *vself, Aoid *oid, void *vserver, int command){
	Device *self = (Device *) vself;
	Acpls *server = (Acpls *) vserver;
	switch(command){
		case CMD_AOID_START:
			device_start(self);
			acpls_reset(server);
			return;
		case CMD_AOID_STOP:
			device_stop(self);
			acpls_reset(server);
			return;
		case CMD_AOID_RESET:
			device_reset(self);
			acpls_reset(server);
			return;
		CASE_AOID_GET_ACP_COMMAND_SUPPORTED(SSR)
	}
	acpls_reset(server);
}

//static void device_serveRequestNone(void *vself, Aoid *oid, void *vserver, int command) {
	//Acpls *server = (Acpls *) vserver;
	//switch(command){
		//CASE_AOID_GET_ACP_COMMAND_SUPPORTED(None)
	//}
	//acpls_reset(server);
//}

static void device_serveAoidRequestGGS(void *vself, Aoid *oid, void *vserver, int command, void (*sendRamParam)(Aoid *, Acpls *, Device *), void (*sendNvramParam)(Aoid *, Acpls *, const DeviceParam *, yn_t), int (*setNvramParam)(Acpls *, DeviceParam *)){
	Device *self = (Device *) vself;
	Acpls *server = (Acpls *) vserver;
	switch(command){
		case CMD_AOID_GET_RAM_VALUE:
			sendRamParam(oid, server, self);
			return;
		case CMD_AOID_GET_NVRAM_VALUE:{
				yn_t success = NO;
				DeviceParam param;
				memset(&param, 0, sizeof param);
				if(pmem_getDeviceParam(&param, self->ind)){
					success = YES;
				}
				sendNvramParam(oid, server, &param, success);
			}
			return;
		case CMD_AOID_SET_NVRAM_VALUE:{
				DeviceParam param;
				if(!pmem_getDeviceParam(&param, self->ind)){
					acpls_reset(server);
					return;
				}
				if(!setNvramParam(server, &param)){
					acpls_reset(server);
					return;
				}
				if(deviceParam_check(&param) == ERROR_NO){
					pmem_saveDeviceParam(&param, self->ind);
				}
			}
			acpls_reset(server);
			return;
		CASE_AOID_GET_ACP_COMMAND_SUPPORTED(GGS)
	}
	acpls_reset(server);
}

static void device_sendRamParamPin(Aoid *oid, Acpls *server, Device *device){aoidServer_sendII(oid, server, device->pin, YES);}
static void device_sendNvramParamPin(Aoid *oid, Acpls *server, const DeviceParam *param, yn_t success){aoidServer_sendII(oid, server, param->pin, success);}
static int device_setNvramParamPin(Acpls *server, DeviceParam *param){int v; if(!acp_packGetCellI(server->acpl->buf, AOID_ACP_REQUEST_IND_PARAM1, &v)){return 0;} param->pin = v; return 1;}
static void device_serveAoidRequestPin(void *vself, Aoid *oid, void *vserver, int command){device_serveAoidRequestGGS(vself, oid, vserver, command, device_sendRamParamPin, device_sendNvramParamPin, device_setNvramParamPin);}

static void device_sendRamParamPollInterval(Aoid *oid, Acpls *server, Device *device){aoidServer_sendUlI(oid, server, (int) device->tmr.interval, YES);}
static void device_sendNvramParamPollInterval(Aoid *oid, Acpls *server, const DeviceParam *param, yn_t success){aoidServer_sendUlI(oid, server, param->poll_interval_ms, success);}
static int device_setNvramParamPollInterval(Acpls *server, DeviceParam *param){unsigned long v; if(!acp_packGetCellUl(server->acpl->buf, AOID_ACP_REQUEST_IND_PARAM1, &v)){return 0;} param->poll_interval_ms = v; return 1;}
static void device_serveAoidRequestPollInterval(void *vself, Aoid *oid, void *vserver, int command){device_serveAoidRequestGGS(vself, oid, vserver, command, device_sendRamParamPollInterval, device_sendNvramParamPollInterval, device_setNvramParamPollInterval);}

void deviceAoid_build(DeviceAoid *self, Aoid *next, Aoid *parent, void *vdevice, size_t *id){
	Device *device = (Device *) vdevice;
	//OBJE_ID_SET_PARAM(OID, 						NEXT,								PARENT,			KIND,								DESCR,									FUNCTION, 								DATA,					ID)
	AOID_SET_PARAM(&self->main,						&self->pin,							parent,			AOID_KIND_COMPLEX,					AOID_DESCRIPTION_DEVICE,				device_serveAoidRequestSelf,			vdevice,				*id)
	AOID_SET_PARAM(&self->pin,						&self->poll_interval,				&self->main,	AOID_KIND_UINT8_PARAM,				AOID_DESCRIPTION_PIN,					device_serveAoidRequestPin,				vdevice,				*id)
	AOID_SET_PARAM(&self->poll_interval,			&device->temperature.aoid.main,		&self->main,	AOID_KIND_TIMEMS_PARAM,				AOID_DESCRIPTION_POLL_INTERVAL_MS,		device_serveAoidRequestPollInterval,	vdevice,				*id)
	channelAoid_build(&device->temperature.aoid, 	&device->humidity.aoid.main,		&self->main,																														&device->temperature,	id);
	channelAoid_build(&device->humidity.aoid, 		next, 								&self->main,																														&device->humidity,		id);
}
